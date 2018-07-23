#pragma once
#include "core/align.h"
#include "font.h"

class SkPaint;

namespace graphics
{
    struct StringFormat
    {
        StringFormat() {}
        StringFormat(const font & font) : _font(font) {}

        StringFormat & font(const font & font)
        {
            _font = font;
            return *this;
        }

        StringFormat & color(core::color32 color)
        {
            _color = color;
            return *this;
        }

        StringFormat & lcd(bool lcd)
        {
            _lcd = lcd;
            return *this;
        }

        StringFormat & weight(int32_t weight)
        {
            _font.weight = (graphics::font_weight)weight;
            return *this;
        }

        StringFormat & align(core::align align)
        {
            _align = align;
            return *this;
        }

        StringFormat & hint(bool hint)
        {
            _hint = hint;
            return *this;
        }

        void apply(SkPaint & paint) const;

        graphics::font _font;
        core::color32 _color = core::colors::Black;
        core::align _align = core::align::leftTop;
        bool _lcd = true;
        bool _hint = true;
    };
}
