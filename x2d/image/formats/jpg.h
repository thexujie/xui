#pragma once

#include "image/image.h"

namespace core::image::formats
{
    struct jpeg_image_info
    {
        jpeg_image_info()
        {
            dwWidth = 0;
            dwHeight = 0;
            dwColorNum = 0;
            dwByteNum = 0;
        }

        unsigned int dwWidth;
        unsigned int dwHeight;
        unsigned int dwColorNum;
        unsigned int dwByteNum;
    };

    const uint8_t JPEG_HEADER[2] = { 0xFF, 0xD8 };
    const uint8_t JPEG_TAIL[2] = { 0xFF, 0xD9 };

    bool is_jpg_data(const byte_t * buffer, size_t length);
    core::error jpg_create(image_codec_context & ictx, const byte_t * buffer, size_t length, image_data_t & image);

    bool jpg_rule_default(image_codec_context * rule);

    const image_convert_fun image_convert_jpeg_use_src = (image_convert_fun)- 1;
}
