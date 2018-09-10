#pragma once

#include "core/core.h"
#include "drawing/drawing.h"

class SkTypeface;

struct hb_font_t;
struct hb_buffer_t;

namespace icu
{
    class BreakIterator;
}

namespace drawing::script
{
    struct item
    {
        section trange;
        // hb_script_t
        uint32_t script = 0;
        uint8_t level = 0;
        uint16_t font = 0;
        uint32_t color = 0;
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
        core::vec2f advance;
        core::vec2f offset;
        bool wordbreak = false;
        bool charbreak = false;
        bool standalone = false;

        uint32_t sindex = 0;
        core::vec2f pos;
    };

    struct segment
    {
        uint32_t sindex = 0;
        section trange;
        section grange;
        uint32_t item = 0;
        uint32_t line = 0;
        float32_t width = 0;

        float32_t offset = 0;
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

        core::error build(SkTextBlobBuilder & builder, uint32_t index);

        core::si32f lineSize(uint32_t index);

        void setFont(section range, const drawing::font & font);
        void setColor(section range, uint32_t color);

        uint16_t fontIndex(const drawing::font & font);

        const drawing::font & font_at(uint16_t index) { return _fonts[index].font; }
        std::shared_ptr<SkTypeface> skfont_at(uint16_t index) { return _fonts[index].skfont; }
        std::shared_ptr<hb_font_t> hbfont_at(uint16_t index) { return _fonts[index].hbfont; }
        const drawing::fontmetrics & fontmetrics_at(uint16_t index) { return _fonts[index].fmetrics; }

    public:
        const glyph & findGlyph(size_t index) const;
        core::rc32f charRect(size_t index) const;

    private:
        constexpr static uint16_t font_default = 0;

        uint8_t _defaultBidiLevel = 0xfe;
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

        std::unique_ptr<hb_buffer_t, void(*)(hb_buffer_t *)> _hbbuffer = {nullptr, nullptr };
        std::unique_ptr<icu::BreakIterator, void(*)(icu::BreakIterator *)> _breaker_world = { nullptr, nullptr};
        std::unique_ptr<icu::BreakIterator, void(*)(icu::BreakIterator *)> _breaker_character = { nullptr, nullptr };

        std::vector<item> _items;
        std::vector<glyph> _glyphs;
        std::vector<segment> _segments;
        std::vector<row> _rows;
    };
}
