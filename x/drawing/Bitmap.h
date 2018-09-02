#pragma once

class SkBitmap;

namespace drawing
{
    class Bitmap
    {
    public:
        Bitmap() = default;
        ~Bitmap() = default;

        Bitmap(const core::si32i & size);

        operator bool() const { return !!_native; }
        bitmap_buffer buffer() const;
        core::si32i size() const;
        core::error Save(std::string path, image::image_type type = image::image_type_none, int32_t quality = 100);

        std::shared_ptr<SkBitmap> native_shared() { return _native; }
        SkBitmap & native() { return *_native; }
        const SkBitmap & native() const { return *_native; }
        SkBitmap * native_ptr() { return _native.get(); }
        const SkBitmap * native_ptr() const { return _native.get(); }
    private:
        core::error _state = core::error_ok;
        std::shared_ptr<SkBitmap> _native = nullptr;
        core::si32i _size;
    };
}