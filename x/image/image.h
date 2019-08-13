#pragma once

#include "format.h"
#include "converter.h"

namespace core
{
	core::error image_load(const std::u8string & path, image_data_t & img);
	core::error image_load(image_codec_context & ictx, const std::u8string & path, image_data_t & img);

	image_type image_get_type_from_ext(const char * ext, size_t length = core::npos);
	image_type image_get_type(const byte_t * buffer, size_t length);

	core::error image_create(const byte_t * buffer, int32_t length, image_data_t & img);
	core::error image_create(image_codec_context & ictx, const byte_t * buffer, int32_t length, image_data_t & img);
}

