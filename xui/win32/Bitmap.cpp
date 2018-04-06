#include "stdafx.h"
#include "Bitmap.h"
#include "win32/windows.h"

namespace win32
{
    Bitmap::Bitmap(core::math::si32_t size)
    {
        HDC hdcScreen = ::GetDC(NULL);
        handle_t hdc = ::CreateCompatibleDC(hdcScreen);
        ::ReleaseDC(NULL, hdcScreen);


        int32_t strike = 4;
        int32_t pitch = size.cx * strike;
        if (pitch & 0x4)
            pitch += 0x4 - (pitch & 0x4);

        BITMAPINFO bmpInfo = { 0 };
        bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmpInfo.bmiHeader.biWidth = size.cx;
        bmpInfo.bmiHeader.biHeight = -size.cy;
        bmpInfo.bmiHeader.biPlanes = 1;
        bmpInfo.bmiHeader.biBitCount = (uint16_t)(strike << 3);
        bmpInfo.bmiHeader.biCompression = BI_RGB;
        bmpInfo.bmiHeader.biSizeImage = (uint32_t)(pitch * size.cy);

        byte_t * data = nullptr;
        handle_t bitmap = ::CreateDIBSection((HDC)hdc, &bmpInfo, DIB_RGB_COLORS, (void **)&data, NULL, 0);
        if (bitmap)
        {
            _hdc = hdc;
            _bitmap = bitmap;
            _data = data;
            _pitch = pitch;
            _size = size;
            ::SelectObject((HDC)_hdc, (HGDIOBJ)_bitmap);
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

    void Bitmap::BitBltTo(handle_t hdc, int32_t x, int32_t y, int32_t width, int32_t height)
    {
        if (_hdc)
            ::BitBlt((HDC)hdc, x, y, width, height, (HDC)_hdc, 0, 0, SRCCOPY);
    }
}
