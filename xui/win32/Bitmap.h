#pragma once

#include "win32.h"
#include "graphics/Pixmap.h"
#include "core/math/vec2.h"

namespace win32
{
    class Bitmap : public graphics::IPixmap
    {
    public:
        Bitmap() = default;
        ~Bitmap() = default;

        Bitmap(core::math::si32_t size);

    public: // IBuffer
        byte_t * data() const { return _data; }
        core::math::si32_t size() const { return _size; }
        int32_t pitch() const { return _pitch; }
        handle_t hdc() const { return _hdc; }
        handle_t handle() const { return _handle; }

        void Clear(core::color32 color);
        void BitBltTo(handle_t hdc, int32_t x, int32_t y, int32_t width, int32_t height);

    public:
        core::error_e Save(std::string path) const;

    private:
        byte_t * _data = nullptr;
        int32_t _strike = 0;
        int32_t _pitch = 0;
        core::math::si32_t _size;

        handle_t _hdc = nullptr;
        handle_t _handle = nullptr;
    };
}
