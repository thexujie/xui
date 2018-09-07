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

namespace drawing::script
{
    struct item
    {
        section trange;
        hb_script_t script;
        bool rtl;
        uint16_t font;
        uint32_t color;
#ifdef _DEBUG
        std::string _text;
        drawing::font _font;
#endif
    };

    struct glyph
    {
#ifdef _DEBUG
        std::string _text;
#endif
        section trange;
        uint16_t gid = 0;
        uint16_t gcount = 0;
        core::vec2<float32_t> advance;
        core::vec2<float32_t> offset;
        bool softbreak = false;
        bool standalone = false;
    };

    struct segment
    {
        section trange;
        section grange;
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
        section trange;
        section grange;
        section srange;
        uint32_t line = 0;
        float32_t width = 0;
        float32_t ascent = 0;
        float32_t descent = 0;
#ifdef _DEBUG
        std::string _text;
#endif
    };

    class Shaper : public IShaper
    {
    public:
        Shaper() = default;
        Shaper(const drawing::font & font, core::color32 color) : _font(font), _color(color){}

        core::error reset(std::string text);
        core::error itermize();
        core::error wrap(float32_t end, wrap_mode mode);

        core::error shape(SkTextBlobBuilder & builder, uint32_t index);

        core::si32f lineSize(uint32_t index);

        void setFont(section range, const drawing::font & font);
        void setColor(section range, uint32_t color);

        uint16_t fontIndex(const drawing::font & font);

        const drawing::font & font_at(uint16_t index) { return _fonts[index].font; }
        std::shared_ptr<SkTypeface> skfont_at(uint16_t index) { return _fonts[index].skfont; }
        std::shared_ptr<hb_font_t> hbfont_at(uint16_t index) { return _fonts[index].hbfont; }
        const drawing::fontmetrics & fontmetrics_at(uint16_t index) { return _fonts[index].fmetrics; }

    private:
        constexpr static uint16_t font_default = 0;

        UBiDiLevel _defaultBidiLevel = UBIDI_DEFAULT_LTR;
        drawing::font _font;
        core::color32 _color = core::colors::Black;
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
        std::vector<section> _chars;
        std::vector<item> _items;
        std::vector<glyph> _glyphs;
        std::vector<segment> _segments;
        std::vector<row> _rows;
    };
}
