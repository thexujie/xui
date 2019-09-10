#pragma once

#include "drawing.h"

class SkPaint;

namespace drawing
{
    struct StringFormat
    {
        StringFormat() {}
        StringFormat(const font & font_) : _font(font_) {}

        StringFormat & font(const font & font)
        {
            _font = font;
            return *this;
        }

        StringFormat & color(core::color color_)
        {
            _color = color_;
            return *this;
        }

        StringFormat & lcd(bool lcd_)
        {
            _lcd = lcd_;
            return *this;
        }

        StringFormat & align(core::align align_)
        {
            _align = align_;
            return *this;
        }

        StringFormat & hint(bool hint_)
        {
            _hint = hint_;
            return *this;
        }

        void apply(SkPaint & paint) const;

        drawing::font _font;
        core::color _color = core::colors::Black;
        core::aligns _align = core::align::leftTop;
        bool _lcd = true;
        bool _hint = true;
    };
}
