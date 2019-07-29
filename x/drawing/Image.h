#pragma once

class SkImage;

namespace drawing
{
    class Image final
    {
    public:
        Image() = default;
        Image(const Image & another);
        explicit Image(const std::u8string & path);
        ~Image();

        operator bool() const { return !!_native; }
        int32_t width() const;
        int32_t height() const;
        core::sizei size() const;
        // width / height
        float32_t aspect() const;
        //image::format cmode() const { return _image.format.format; }
        core::error Save(std::string path, image::image_type type = image::image_type_none, int32_t quality = 100) const;

        std::shared_ptr<SkImage> native_shared() { return _native; }
        SkImage & native() { return *_native; }
        const SkImage & native() const { return *_native; }
        SkImage * native_ptr() { return _native.get(); }
        const SkImage * native_ptr() const { return _native.get(); }
    private:
        std::shared_ptr<SkImage> _native;
    };
}
