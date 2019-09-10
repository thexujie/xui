#pragma once

#include "drawing.h"

class SkColorFilter;
namespace drawing
{
    class ColorFilter
    {
    public:
        ColorFilter() = default;
        ColorFilter(std::shared_ptr<SkColorFilter> native) : _native(native) {}

        std::shared_ptr<SkColorFilter> native_shared() { return _native; }
        SkColorFilter & native() { return *_native; }
        const SkColorFilter & native() const { return *_native; }
        SkColorFilter * native_ptr() { return _native.get(); }
        const SkColorFilter * native_ptr() const { return _native.get(); }

    private:
        std::shared_ptr<SkColorFilter> _native;

    public:
		static std::shared_ptr<ColorFilter> createBlendMode(core::color color, blend_mode mode);
		static std::shared_ptr<ColorFilter> createGrayScale(float32_t brightness = 1.0f, float32_t alpha = 1.0f);
    };
}
