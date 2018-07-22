#pragma once
#include "GraphicsService.h"

class SkImage;

namespace graphics
{
    class Image final
    {
    public:
        Image() = default;
        ~Image();
        Image(std::string path);

        operator bool() const { return !!_image.data; }
        core::si32i size() const { return { _image.format.width, _image.format.height}; }
        image::format cmode() const { return _image.format.format; }
        const image::image_data_t & data() const { return _image; }
        core::error_e Save(std::string path, image::image_type type = image::image_type_none, int32_t quality = 100) const;

    private:
        image::image_data_t _image;
        std::shared_ptr<SkImage> _native;
    };
}
