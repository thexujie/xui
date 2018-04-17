#include "stdafx.h"
#include "Image.h"
#include "graphics/image/image.h"
#include "graphics/image/formats/dds.h"
#include "graphics/image/formats/jpg.h"
#include "core/io/filestream.h"
#include "graphics/image/formats/bmp.h"
#include "graphics/image/formats/tga.h"
#include "win32/windows.h"

namespace win32
{
    using namespace core;
    using namespace graphics;

    Image::Image(std::shared_ptr<HDC> hdc, std::string path):_hdc(hdc)
    {
        LoadFromFile(path);
    }

    core::error_e Image::LoadFromFile(std::string path)
    {
        auto[data, size] = core::io::readFullFile(path);
        if (!data || size <= 0)
            return error_io;

        graphics::image::image_t img;
        graphics::image::image_create(data.get(), (int32_t)size, img);

        if (img.data.format.format == graphics::image::format_a8r8g8b8 || img.data.format.format == graphics::image::format_x8r8g8b8)
        {
            BITMAPINFO bmpInfo = { 0 };
            bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmpInfo.bmiHeader.biWidth = img.data.format.width;
            bmpInfo.bmiHeader.biHeight = -img.data.format.height;
            bmpInfo.bmiHeader.biPlanes = 1;
            bmpInfo.bmiHeader.biBitCount = 32;
            bmpInfo.bmiHeader.biCompression = BI_RGB;
            bmpInfo.bmiHeader.biSizeImage = img.data.format.height * img.data.pitch;

            byte_t * bits = nullptr;
            HBITMAP hBitmap = ::CreateDIBSection(*_hdc.get(), &bmpInfo, DIB_RGB_COLORS, (void **)&bits, NULL, 0);
            buffcpy8(bits, bmpInfo.bmiHeader.biSizeImage, img.data.data, img.data.format.height * img.data.pitch);
            if(img.data.format.format == graphics::image::format_a8r8g8b8)
            {
                // µÃÔ¤³Ë
                for(int row = 0; row < img.data.format.height; ++row)
                {
                    byte_t * line = bits + row * img.data.pitch;
                    for(int col = 0; col < img.data.format.width; ++col)
                    {
                        image::color_a8r8g8b8_t * pixel = (image::color_a8r8g8b8_t *)(line + sizeof(image::color_a8r8g8b8_t) * col);
                        pixel->r = pixel->r * pixel->a / 0xff;
                        pixel->g = pixel->g * pixel->a / 0xff;
                        pixel->b = pixel->b * pixel->a / 0xff;
                        //pixel->a = pixel->a * pixel->a / 0xff;
                    }
                }
            }
            _data = bits;
            _strike = 4;
            _pitch = img.data.pitch;
            _handle = hBitmap;
            _size = { img.data.format.width, img.data.format.height };
            _cmode = img.data.format.format;
        }
        img.pfn_free(img.data);
        return error_ok;
    }

    core::error_e Image::Save(std::string path) const
    {
        if (!_handle)
            return error_state;

        std::fstream fs;
        fs.open(path, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!fs.good())
            return error_io;

        BITMAPFILEHEADER bmfHdr = {};
        bmfHdr.bfType = 0x4D42;
        bmfHdr.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + _pitch * _size.cy;
        bmfHdr.bfReserved1 = 0;
        bmfHdr.bfReserved2 = 0;
        bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

        BITMAPINFOHEADER bi = {};
        bi.biSize = sizeof(BITMAPINFOHEADER);
        bi.biWidth = _size.cx;
        bi.biHeight = -_size.cy;
        bi.biPlanes = 1;
        bi.biBitCount = _strike * 8;
        bi.biCompression = BI_RGB;
        bi.biSizeImage = 0;
        bi.biXPelsPerMeter = 0;
        bi.biYPelsPerMeter = 0;
        bi.biClrImportant = 0;
        bi.biClrUsed = 0;

        fs.write((const char *)&bmfHdr, sizeof(BITMAPFILEHEADER));
        fs.write((const char *)&bi, sizeof(BITMAPINFOHEADER));
        fs.write((const char *)_data, _pitch * _size.cy);

        fs.close();
        return error_ok;
    }
}
