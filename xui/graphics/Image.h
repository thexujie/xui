#pragma once
#include "GraphicsService.h"

namespace graphics
{
    class Image final : public IGraphicsImage
    {
    public:
        Image() = default;
        ~Image();
        Image(std::string path);

        operator bool() const { return !!_image.data.data; }
        si32_t size() const { return { _image.data.format.width, _image.data.format.height}; }
        image::format cmode() const { return _image.data.format.format; }
        const image::image_t & image() const { return _image; }
        core::error_e Save(std::string path) const;

    private:
        image::image_t _image;
    };
}
