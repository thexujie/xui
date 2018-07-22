#pragma once

#include "GraphicsService.h"

class SkBitmap;

namespace graphics
{
    class Bitmap
    {
    public:
        Bitmap() = default;
        ~Bitmap() = default;

        Bitmap(core::si32i size);

        operator bool() const { return !!_native; }
        bitmap_buffer buffer() const;
        core::si32i size() const;
        core::error_e Save(std::string path, image::image_type type = image::image_type_none, int32_t quality = 100);

        std::shared_ptr<SkBitmap> native_share() { return _native; }
        SkBitmap & native() { return *_native; }
        const SkBitmap & native() const { return *_native; }
    private:
        core::error_e _state = core::error_ok;
        std::shared_ptr<SkBitmap> _native = nullptr;
        core::si32i _size;
    };
}
