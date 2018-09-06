#include "stdafx.h"
#include "Bitmap.h"
#include "win32/windows.h"

namespace win32
{
    using namespace core;

    Bitmap::Bitmap(std::shared_ptr<HDC> hdc):_hdc(hdc)
    {
        
    }

    Bitmap::Bitmap(std::shared_ptr<HDC> hdc, core::si32i size) : _hdc(hdc)
    {
        int32_t strike = 4;
        int32_t pitch = size.cx * strike;
        if (pitch & 0x4)
            pitch += 0x4 - (pitch & 0x4);

        BITMAPINFO bmpInfo = { 0 };
        bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmpInfo.bmiHeader.biWidth = size.cx;
        bmpInfo.bmiHeader.biHeight = -size.cy;
        bmpInfo.bmiHeader.biPlanes = 1;
        bmpInfo.bmiHeader.biBitCount = (uint16_t)(strike * 8);
        bmpInfo.bmiHeader.biCompression = BI_RGB;
        bmpInfo.bmiHeader.biSizeImage = (uint32_t)(pitch * size.cy);

        byte_t * data = nullptr;
        HBITMAP bitmap = ::CreateDIBSection(*hdc, &bmpInfo, DIB_RGB_COLORS, (void **)&data, NULL, 0);
        if (bitmap)
        {
            _handle = bitmap;
            _data = data;
            _strike = strike;
            _pitch = pitch;
            _size = size;
            ::SelectObject(*_hdc.get(), (HGDIOBJ)_handle);
        }
    }

    void Bitmap::Clear(core::color32 color)
    {
        if (!_data)
            return;

        for (int32_t row = 0, col = 0; row != _size.cy; ++row)
        {
            byte_t * line = _data + row * _pitch;
            core::buffset32(line, _size.cx, color.argb);
        }
    }

    void Bitmap::BitBltTo(HDC hdc, int32_t x, int32_t y, int32_t width, int32_t height)
    {
        if (_hdc && hdc != *_hdc)
            ::BitBlt(hdc, x, y, width, height, *_hdc.get(), 0, 0, SRCCOPY);
    }

    drawing::bitmap_buffer Bitmap::buffer() const
    {
        return { _data, _size, _strike, _pitch, false };
    }

    core::error Bitmap::Save(std::string path) const
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
