#include "stdafx.h"
#include "ico.h"

#include "bmp.h"
#include "png.h"

namespace core::image::formats
{
    int32_t ico_create(image_codec_context & ictx, byte_t * buffer, int32_t length, image_data_t & image, int32_t index)
    {
        image_clear(&image, sizeof(image_data_t));

        if (!buffer || length < 0 || index < 0)
            return 1;

        icon_header_t * icon = (icon_header_t *)buffer;
        if (icon->reserved || icon->plane_count < 1 || index < 0 || index >= icon->plane_count)
            return 2;

        icon_plane_header_t * plane = (icon_plane_header_t *)(buffer + sizeof(icon_header_t));
        plane += index;

        // png
        if (plane->width == 0 && plane->height == 0)
        {
            return png_create(ictx, buffer + plane->data_offset, plane->data_size, image);
        }
        buffer += plane->data_offset;

        bmp_info_windows_t * info = (bmp_info_windows_t *)buffer;

        if (info->size != sizeof(bmp_info_windows_t))
            return 3;

        buffer += sizeof(bmp_info_windows_t);

        int32_t flags = 0;
        if (info->height > 0)
            flags |= IMAGE_CONVERT_FLIP_Y;

        format src_mode = format::none;
        byte_t * pallete = nullptr;
        byte_t * src = buffer;
        switch (info->bit_count)
        {
        case 4:
            src_mode = format::index4_a8r8g8b8;
            pallete = buffer;
            src = pallete + 16 * 4;
            break;
        case 8:
            src_mode = format::index8_a8r8g8b8;
            pallete = buffer;
            src = pallete + 256 * 4;
            break;
        case 16:
            src_mode = format::x1r5g5b5;
            break;
        case 24:
            src_mode = format::r8g8b8;
            break;
        case 32:
            src_mode = format::a8r8g8b8;
            break;
        default:
            return 4;
        }

		image_convert_fun pfn_convert = image_convert_ex;
		image_format format;
		format.width = (int32_t)plane->width;
		format.height = (int32_t)plane->height;
		format.format = src_mode;
		if (ictx.get_format)
			image.format = ictx.get_format(image_type_bmp, format);

		ictx.pfn_alloc(image, 4);
		image.pfn_free = ictx.pfn_free;

		image_data_t src_data = {};
		src_data.format = format;
		src_data.data = const_cast<byte_t *>(src);
		src_data.pitch = align_to<int32_t>(format.width * (format_bits(format.format) / 8), 4);
		src_data.palette = const_cast<byte_t *>(pallete);
		src_data.palette_size = 0;

		error err = pfn_convert(ictx, src_data, image);
		if (err < 0)
		{
			image.pfn_free(image);
			return err;
		}
		return ok;
    }
}
