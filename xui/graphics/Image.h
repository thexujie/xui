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

        operator bool() const { return !!_image.data; }
        si32_t size() const { return { _image.format.width, _image.format.height}; }
        image::format cmode() const { return _image.format.format; }
        const image::image_data_t & image() const { return _image; }
        core::error_e Save(std::string path) const;

    private:
        image::image_data_t _image;
    };
}
