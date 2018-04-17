#pragma once
#include "GraphicsService.h"

namespace graphics
{
    class Image
    {
    public:
        Image() = default;
        ~Image();
        Image(std::string path);

        std::shared_ptr<IGraphicsImage> image() const { return _image; }
        si32_t size() const;

        operator bool() const { return !!_image; }

        core::error_e Save(std::string path) const;

    private:
        image::image_t _img;
        std::shared_ptr<IGraphicsImage> _image;
    };
}
