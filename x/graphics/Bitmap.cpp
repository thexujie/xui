#include "stdafx.h"
#include "Bitmap.h"

#include "skia/skia.h"

namespace graphics
{
    Bitmap::Bitmap(core::si32i size)
    {
        auto bitmap = std::make_shared<SkBitmap>();
        bitmap->allocN32Pixels(size.cx, size.cy);
        _native = bitmap;
    }

    bitmap_buffer Bitmap::buffer() const
    {
        if (!_native)
            return {};

        bitmap_buffer buffer = {};
        buffer.data = static_cast<byte_t *>(_native->getPixels());
        buffer.size = skia::size(_native->dimensions());
        buffer.strike = 4;
        buffer.pitch = static_cast<int32_t>(_native->rowBytes());
        return buffer;
    }

    core::si32i Bitmap::size() const
    {
        if (!_native)
            return {};
        return skia::size(_native->dimensions());
    }

    core::error_e Bitmap::Save(std::string path, image::image_type type, int32_t quality)
    {
        if (!_native)
            return core::error_nullptr;

        if (type == image::image_type_none)
            type = image::image_get_type_from_ext(std::filesystem::path(path).extension().string().c_str());

        SkFILEWStream stream(path.c_str());
        return SkEncodeImage(&stream, *_native, skia::to(type), quality) ? core::error_ok : core::error_inner;
    }
}
