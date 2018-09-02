#pragma once

#include "core/core.h"
#include "drawing/drawing.h"

#include <hb-ot.h>
#include <unicode/brkiter.h>
#include <unicode/locid.h>
#include <unicode/stringpiece.h>
#include <unicode/ubidi.h>
#include <unicode/uchriter.h>
#include <unicode/unistr.h>
#include <unicode/uscript.h>


#include "SkFontMgr.h"
#include "SkStream.h"
#include "SkTextBlob.h"
#include "SkTypeface.h"

namespace script
{
    // range
    struct range
    {
        int32_t index = -1;
        int32_t length = 0;

        int32_t end() const { return index + length; }
        range operator + (const range & rhs)
        {
            if (index < 0 || !length)
                return rhs;

            if (rhs.index < 0 || !rhs.length)
                return *this;

            if (index + length == rhs.index)
                return { index, rhs.index + rhs.length };

            if (rhs.index + rhs.length == index)
                return { rhs.index, index + length };

            return { -1, 0 };
        }

        range & operator += (const range & rhs)
        {
            if (index < 0 || !length)
                *this = rhs;
            else if (rhs.index < 0 || !rhs.length)
                ;
            else if (index + length == rhs.index)
                *this = { index, length + rhs.length };
            else if (rhs.index + rhs.length == index)
                *this = { rhs.index, length + rhs.length };
            else
                *this = { -1, 0 };
            return *this;
        }
    };

    struct item
    {
        range range;
        hb_script_t script;
        bool rtl;
        uint16_t font;
        uint32_t color;
#ifdef _DEBUG
        std::u16string _text;
        drawing::font _font;
#endif
    };

    class Shaper
    {
    public:
        Shaper() = default;

        core::error reset(std::string text);
        core::error itermize();

        void setFont(range range, const drawing::font & font);
        void setColor(range range, uint32_t color);

        uint16_t fontIndex(const drawing::font & font);
    private:
        constexpr static uint16_t font_default = 0;
        constexpr static uint32_t color_default = core::colors::Black.argb;
        UBiDiLevel _defaultBidiLevel = UBIDI_DEFAULT_LTR;
        drawing::font _font;
        std::string _text;
        std::vector<item> _items;
        std::unordered_map<drawing::font, uint16_t> _font_indices;
        std::vector<std::pair<drawing::font, std::shared_ptr<SkTypeface>>> _fonts;
        std::vector<uint16_t> _rtf_font_indices;
        std::vector<uint32_t> _rtf_colors;
    };
}
