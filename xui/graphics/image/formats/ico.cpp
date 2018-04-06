#include "stdafx.h"
#include "ico.h"

#include "bmp.h"
#include "png.h"

namespace graphics { namespace image { namespace formats
{
    int32_t ico_create(byte_t * buffer, int32_t length, image_data_t * img, int32_t index)
    {
        image_clear(img, sizeof(image_data_t));

        if (!buffer || length < 0 || !img || index < 0)
            return 1;

        icon_header_t * icon = (icon_header_t *)buffer;
        if (icon->reserved || icon->plane_count < 1 || index < 0 || index >= icon->plane_count)
            return 2;

        icon_plane_header_t * plane = (icon_plane_header_t *)(buffer + sizeof(icon_header_t));
        plane += index;

        // png
        if (plane->width == 0 && plane->height == 0)
        {
            return png_create(buffer + plane->data_offset, plane->data_size, img);
        }
        buffer += plane->data_offset;

        bmp_info_windows_t * info = (bmp_info_windows_t *)buffer;

        if (info->size != sizeof(bmp_info_windows_t))
            return 3;

        buffer += sizeof(bmp_info_windows_t);

        int32_t flags = 0;
        if (info->height > 0)
            flags |= IMAGE_CONVERT_FLIP_Y;

        cmode_e src_mode = cmode_invalid;
        byte_t * pallete = nullptr;
        byte_t * src = buffer;
        switch (info->bit_count)
        {
        case 4:
            src_mode = cmode_index4_a8r8g8b8;
            pallete = buffer;
            src = pallete + 16 * 4;
            break;
        case 8:
            src_mode = cmode_index8_a8r8g8b8;
            pallete = buffer;
            src = pallete + 256 * 4;
            break;
        case 16:
            src_mode = cmode_x1r5g5b5;
            break;
        case 24:
            src_mode = cmode_r8g8b8;
            break;
        case 32:
            src_mode = cmode_a8r8g8b8;
            break;
        default:
            return 4;
        }

        int32_t width = (int32_t)plane->width;
        int32_t height = (int32_t)plane->height;
        image_convert_rule_t rule = {image_format_bmp, width, height, src_mode, nullptr};
        if (bmp_rule_default(&rule))
        {
            image_data_t bmp;
            bmp.width = rule.width;
            bmp.height = rule.height;
            bmp.bits = rule.dst_bits;
            bmp.pitch = rule.dst_pitch;
            bmp.length = bmp.height * bmp.pitch;
            bmp.buffer = image_malloc(bmp.length);
            bmp.src_mode = rule.src_mode;
            bmp.dst_mode = rule.dst_mode;
            bmp.flags = flags;

            rule.image_convert_fun(rule.width, rule.height,
                                   rule.pixel_convert_fun,
                                   pallete, rule.pal_stride,
                                   src, rule.src_stride, rule.src_pitch,
                                   bmp.buffer, rule.dst_stride, rule.dst_pitch, flags);
            *img = bmp;
            return true;
        }
        return false;
    }
}}}
