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
#include <bitset>

namespace script
{
    enum class wrap_mode
    {
        none = 0,
        word,
        charactor,
    };

    // range
    struct range
    {
        size_t index = 0;
        size_t length = 0;

        size_t end() const { return index + length; }
        range operator + (const range & rhs)
        {
            if (!length)
                return rhs;

            if (!rhs.length)
                return *this;

            if (index + length == rhs.index)
                return { index, rhs.index + rhs.length };

            if (rhs.index + rhs.length == index)
                return { rhs.index, index + length };

            return { 0, 0 };
        }

        range & operator += (const range & rhs)
        {
            if (!length)
                *this = rhs;
            else if (!rhs.length)
                ;
            else if (index + length == rhs.index)
                *this = { index, length + rhs.length };
            else if (rhs.index + rhs.length == index)
                *this = { rhs.index, length + rhs.length };
            else
                *this = { 0, 0 };
            return *this;
        }
    };

    struct item
    {
        range trange;
        hb_script_t script;
        bool rtl;
        uint16_t font;
        uint32_t color;
#ifdef _DEBUG
        std::u32string _text;
        drawing::font _font;
#endif
    };

    struct glyph
    {
#ifdef _DEBUG
        std::string _text;
#endif
        range trange;
        uint16_t gid = 0;
        uint16_t gcount = 0;
        core::vec2<float32_t> advance;
        core::vec2<float32_t> offset;
        bool softbreak = false;
        bool standalone = false;
    };

    struct segment
    {
        range trange;
        range grange;
        uint32_t item = 0;
        uint32_t line = 0;
        float32_t offset = 0;
        float32_t width = 0;
#ifdef _DEBUG
        std::string _text;
#endif
    };

    struct row
    {
        range trange;
        range grange;
        range srange;
        uint32_t line = 0;
        float32_t width = 0;
#ifdef _DEBUG
        std::string _text;
#endif
    };

    class Shaper
    {
    public:
        Shaper() = default;

        core::error reset(std::string text);
        core::error itermize();
        core::error wrap(float32_t end, wrap_mode mode);

        core::error shape(SkTextBlobBuilder & builder, uint32_t index);

        void setFont(range range, const drawing::font & font);
        void setColor(range range, uint32_t color);

        uint16_t fontIndex(const drawing::font & font);
        const drawing::font & font_at(uint16_t index) { return _fonts[index].font; }
        std::shared_ptr<SkTypeface> skfont_at(uint16_t index) { return _fonts[index].skfont; }
        std::shared_ptr<hb_font_t> hbfont_at(uint16_t index) { return _fonts[index].hbfont; }
        const drawing::fontmetrics & fontmetrics_at(uint16_t index) { return _fonts[index].fmetrics; }

    private:
        constexpr static uint16_t font_default = 0;
        constexpr static uint32_t color_default = core::colors::Black.argb;
        UBiDiLevel _defaultBidiLevel = UBIDI_DEFAULT_LTR;
        drawing::font _font;
        std::string _text;

        std::unordered_map<drawing::font, uint16_t> _font_indices;
        struct font_cache
        {
            drawing::font font;
            std::shared_ptr<SkTypeface> skfont;
            std::shared_ptr<hb_font_t> hbfont;
            drawing::fontmetrics fmetrics;
        };
        std::vector<font_cache> _fonts;
        std::vector<uint16_t> _rtf_font_indices;
        std::vector<uint32_t> _rtf_colors;

        std::unique_ptr<hb_buffer_t, decltype(&hb_buffer_destroy)> _hbbuffer = {nullptr, hb_buffer_destroy };
        std::unique_ptr<icu::BreakIterator> _breaker;
        std::vector<UBiDiLevel> _bidis;
#ifdef _DEBUG
        std::u32string _u32text;
#endif
        std::vector<range> _chars;
        std::vector<item> _items;
        std::vector<glyph> _glyphs;
        std::vector<segment> _segments;
        std::vector<row> _rows;
    };
}
