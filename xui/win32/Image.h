#pragma once
#include "graphics/GraphicsService.h"
#include "win32/windows.h"

namespace win32
{
    class Image : public graphics::IGraphicsImage
    {
    public:
        Image(std::shared_ptr<HDC> hdc) : _hdc(hdc) {}
        ~Image() = default;
        Image(std::shared_ptr<HDC> hdc, std::string path);

        core::math::si32_t size() const { return _size; }
        graphics::image::format cmode() const { return _cmode; }

        core::error_e LoadFromFile(std::string path);
        core::error_e Save(std::string path) const;

    public:
        std::shared_ptr<HDC> hdc() const { return _hdc; }
        HBITMAP bitmap() const { return _handle; }

    private:
        std::shared_ptr<HDC> _hdc;

        byte_t * _data = nullptr;
        int32_t _strike = 0;
        int32_t _pitch = 0;

        HBITMAP _handle = nullptr;

        graphics::image::format _cmode = graphics::image::format_none;
        core::math::si32_t _size;
    };
}
