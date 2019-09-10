#pragma once

#include "image.h"

namespace core
{
	core::error image_load(const std::u8string & path, image_data_t & img);
	core::error image_load(image_codec_context & ictx, const std::u8string & path, image_data_t & img);

	core::error image_create(const byte_t * buffer, int32_t length, image_data_t & img);
	core::error image_create(image_codec_context & ictx, const byte_t * buffer, int32_t length, image_data_t & img);
}

