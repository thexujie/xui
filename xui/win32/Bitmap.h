#pragma once

#include "graphics/Pixmap.h"
#include "core/math/vec2.h"
#include "win32/windows.h"

namespace win32
{
    class Bitmap : public graphics::IGraphicsPixmap
    {
    public:
        ~Bitmap() = default;

        Bitmap(std::shared_ptr<HDC> hdc);
        Bitmap(std::shared_ptr<HDC> hdc, core::math::si32_t size);

    public: // IBuffer
        byte_t * data() const { return _data; }
        core::math::si32_t size() const { return _size; }
        int32_t pitch() const { return _pitch; }
        std::shared_ptr<HDC> hdc() const { return _hdc; }
        HBITMAP handle() const { return _handle; }

        void Clear(core::color32 color);
        void BitBltTo(HDC hdc, int32_t x, int32_t y, int32_t width, int32_t height);

    public:
        core::error_e Save(std::string path) const;

    private:
        byte_t * _data = nullptr;
        int32_t _strike = 0;
        int32_t _pitch = 0;
        core::math::si32_t _size;

        std::shared_ptr<HDC> _hdc;
        HBITMAP _handle = nullptr;
    };
}
