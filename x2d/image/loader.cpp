#include "stdafx.h"
#include "loader.h"

#include "formats/bmp.h"
#include "formats/tga.h"
#include "formats/png.h"
#include "formats/jpg.h"
#include "formats/dds.h"

namespace core
{
    using namespace core;
    using namespace core::image::formats;

	error image_load(const std::u8string & path, image_data_t & img)
	{
		std::wstring pathw = core::u8str_wstr(path);
		std::fstream fs;
		fs.open(pathw.c_str(), std::ios::in | std::ios::binary);
		if (!fs.good())
			return e_not_found;

		fs.seekg(0, std::ios::end);
		auto length = fs.tellg();
		fs.seekg(0, std::ios::beg);

		std::shared_ptr<byte_t[]> buffer(image_malloc((int32_t)length));
		fs.read(buffer.get(), length);
		if (fs.tellg() != length)
			return e_io;
		fs.close();

		return image_create(buffer.get(), (int32_t)length, img);
	}

	core::error image_load(image_codec_context & ictx, const std::u8string & path, image_data_t & img)
	{
		std::wstring pathw = core::u8str_wstr(path);
		std::fstream fs;
		fs.open(pathw.c_str(), std::ios::in | std::ios::binary);
		if (!fs.good())
			return e_not_found;

		fs.seekg(0, std::ios::end);
		auto length = fs.tellg();
		fs.seekg(0, std::ios::beg);

		std::shared_ptr<byte_t[]> buffer(image_malloc((int32_t)length));
		fs.read(buffer.get(), length);
		if (fs.tellg() != length)
			return e_io;
		fs.close();

		return image_create(ictx, buffer.get(), (int32_t)length, img);
	}

	core::error image_create(const byte_t * buffer, int32_t length, image_data_t & img)
	{
		image_codec_context ictx;
		ictx.get_format = image_get_format;
		ictx.get_sampler = image_get_samapler;
		ictx.pfn_alloc = image_buffer_alloc_default;
		ictx.pfn_free = image_buffer_free;
		return image_create(ictx, buffer, length, img);
	}

	core::error image_create(image_codec_context & ictx, const byte_t * buffer, int32_t length, image_data_t & img)
	{
		if (ictx.type == image_type_none)
			ictx.type = image_get_type(buffer, length);

		switch (ictx.type)
		{
		case image_type_bmp:
			return bmp_create(ictx, buffer, length, img);
		case image_type_tga:
			return tga_create(ictx, buffer, length, img);
		case image_type_png:
			return png_create(ictx, buffer, length, img);
		case image_type_jpeg:
			return jpg_create(ictx, buffer, length, img);
		case image_type_dds:
			return dds_create(ictx, buffer, length, img);
		default:
			return e_bad_format;
		}
	}
}
