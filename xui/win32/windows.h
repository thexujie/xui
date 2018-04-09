#pragma once

#include "win32.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "graphics/text/font.h"
#include "core/math/rect.h"

namespace win32
{
    u8string GUID2String(const GUID & guid);
    u8string GUID2String(std::array<uint8_t, 16> data);

    struct winrect_t : public RECT
    {
        winrect_t() {}
        winrect_t(int32_t x, int32_t y, int32_t w, int32_t h)
        {
            left = (int32_t)x; top = (int32_t)y;
            right = (int32_t)(x + w); bottom = (int32_t)(y + h);
        }
        winrect_t(const core::math::rc32_t & rect)
        {
            left = (int32_t)rect.x; top = (int32_t)rect.y;
            right = (int32_t)rect.right(); bottom = (int32_t)rect.bottom();
        }

        int32_t width() const { return right - left; }
        int32_t height() const { return bottom - top; }
    };

}
