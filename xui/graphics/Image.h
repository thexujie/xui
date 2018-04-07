#pragma once
#include "GraphicsService.h"

namespace graphics
{
    class Image
    {
    public:
        Image() = default;
        ~Image() = default;
        Image(std::string path);

        std::shared_ptr<IImage> image() const { return _image; }
        si32_t size() const;

        operator bool() const { return !!_image; }

        void Save(std::string path) const;

    private:
        std::shared_ptr<IImage> _image;
    };
}
