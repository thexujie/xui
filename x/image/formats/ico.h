#pragma once

#include "image/image.h"

namespace core::image::formats
{
#pragma pack(push, 1)
    struct icon_plane_header_t
    {
        uint8_t width; // width, in pixels, of the image
        uint8_t height; // height, in pixels, of the image
        uint8_t color_count; // number of colors in image (0 if >=8bpp)
        uint8_t reserved; // reserved ( must be 0)
        uint16_t plane_count; // color planes
        uint16_t bi_tcount; // bits per pixel
        uint32_t data_size; // how many bytes in this resource?
        uint32_t data_offset; // where in the file is this image?
    };

    struct icon_header_t
    {
        uint16_t reserved; // reserved (must be 0)
        uint16_t type; // resource type (1 for icons)
        uint16_t plane_count; // how many images?
    };
#pragma pack(pop)

    //typdef struct
    //{
    //	bmp_info_windows_t   icheader;      // dib header
    //	rgbquad         iccolors[1];   // color table
    //	byte            icxor[1];      // dib bits for xor mask
    //	byte            icand[1];      // dib bits for and mask
    //} iconimage, *lpiconimage;

    int32_t ico_create(image_codec_context & ictx, byte_t * buffer, int32_t length, image_data_t & image, int32_t iIndex);
}
