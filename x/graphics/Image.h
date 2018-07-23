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

        operator bool() const { return !!_native; }
        int32_t width() const;
        int32_t height() const;
        core::si32i size() const;
        //image::format cmode() const { return _image.format.format; }
        core::error_e Save(std::string path, image::image_type type = image::image_type_none, int32_t quality = 100) const;

        std::shared_ptr<SkImage> native_shared() { return _native; }
        SkImage & native() { return *_native; }
        const SkImage & native() const { return *_native; }
        SkImage * native_ptr() { return _native.get(); }
        const SkImage * native_ptr() const { return _native.get(); }
    private:
        std::shared_ptr<SkImage> _native;
    };
}
