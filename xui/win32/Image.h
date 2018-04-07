#pragma once
#include "graphics/GraphicsService.h"
#include "win32.h"

namespace win32
{
    class Image : public graphics::IImage
    {
    public:
        Image(handle_t hdc) : _hdc(hdc) {}
        ~Image() = default;
        Image(handle_t hdc, std::string path);

        core::math::si32_t size() const { return _size; }
        graphics::image::cmode_e cmode() const { return _cmode; }

        core::error_e LoadFromFile(std::string path);
        core::error_e Save(std::string path) const;

    public:
        handle_t hdc() const { return _hdc; }
        handle_t bitmap() const { return _handle; }

    private:
        handle_t _hdc = nullptr;

        handle_t _data = nullptr;
        int32_t _strike = 0;
        int32_t _pitch = 0;

        handle_t _handle = nullptr;

        graphics::image::cmode_e _cmode = graphics::image::cmode_none;
        core::math::si32_t _size;
    };
}
