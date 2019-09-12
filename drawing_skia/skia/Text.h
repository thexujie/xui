#pragma once

#include "skia.h"

struct hb_font_t;
struct hb_buffer_t;

namespace icu
{
    class BreakIterator;
}

namespace drawing::skia
{

    class Shaper
    {
    public:
        struct font_cache
        {
            drawing::font font;
            std::shared_ptr<SkTypeface> sktypeface;
            std::shared_ptr<hb_font_t> hbfont;
            drawing::fontmetrics fmetrics;
        };

    public:
        Shaper();
        static Shaper & instance();

        std::shared_ptr<SkTextBlobBuilder> builder();
        icu::BreakIterator & breaker_world() { return *_breaker_world; }
        icu::BreakIterator & breaker_character() { return *_breaker_character; }

        uint16_t indexFont(const drawing::font & font);

        const std::vector<font_cache> & fonts(uint16_t index) const { return _fonts; }
        const font_cache & cache(uint16_t index) const { return _fonts[index]; }
        const drawing::font & font(uint16_t index) { return _fonts[index].font; }
        std::shared_ptr<SkTypeface> & sktypeface(uint16_t index) { return _fonts[index].sktypeface; }
        std::shared_ptr<hb_font_t> & hbfont(uint16_t index) { return _fonts[index].hbfont; }
        const drawing::fontmetrics & fontmetrics(uint16_t index) { return _fonts[index].fmetrics; }

    private:
        std::unordered_map<drawing::font, uint16_t> _font_indices;
        std::vector<font_cache> _fonts;

        std::shared_ptr<SkTextBlobBuilder> _builder;
        std::unique_ptr<icu::BreakIterator, void(*)(icu::BreakIterator *)> _breaker_world = { nullptr, nullptr };
        std::unique_ptr<icu::BreakIterator, void(*)(icu::BreakIterator *)> _breaker_character = { nullptr, nullptr };


    public:
        static inline glyph empty_glyph = {};
        static inline cluster empty_cluster = {};
    };

    class Text : public drawing::Text
    {
    public:
        Text() = default;
        Text(const std::u8string & text) :_text(text) {}
        ~Text() = default;

    public:
        const std::u8string & text() const { return _text; }
        bool empty() const { return _text.empty(); }
        size_t length() const { return _text.length(); }
        const char8_t * c_str() const { return _text.c_str(); }
        Text & insert(size_t off, const char8_t * text, size_t count) { _text.insert(off, text, count); invalidate(); return *this; }
        Text & erase(size_t off, size_t count) { _text.erase(off, count); invalidate(); return *this; }

    public:
        void setText(const std::u8string & text) { _text = text; invalidate(); }
        void setFont(span32 range, const drawing::font & font);
        void setColor(span32 range, uint32_t color);

        std::shared_ptr<SkTextBlob> blob() const { return _blob; }
    public:
        void invalidate() { _blob = {}; }
		core::error update(std::u8string_view text, const drawing::font & font, core::color color);
		//core::error update(const drawing::font & font, core::color color);

    public:
        core::error itermize(const drawing::font & font, core::color color);
        core::error layout();
        core::sizef bounds() const { return _bounds; }
        std::shared_ptr<SkTextBlob> build(float32_t width = 0);

    public:
        const std::vector<glyph> & glyphs() const { return _glyphs; }
        size_t glyphCount() const { return _glyphs.size(); }
        const glyph & glyphAt(size_t tindex) const { if (tindex >= _glyphs.size()) return Shaper::empty_glyph; return _glyphs.at(tindex); }

        const std::vector<cluster> & clusters() const { return _clusters; }
        size_t clusterCount() const { return _clusters.size(); }
        const cluster & clusterAt(size_t cindex) const { if (cindex >= _clusters.size()) return Shaper::empty_cluster; return _clusters.at(cindex); }
        const cluster & findCluster(size_t tindex) const;
        const cluster & findCluster(float32_t pos) const;

        std::tuple<size_t, core::rectf> textRect(size_t toffset, size_t tlength) const;

    protected:
        Shaper & _shaper = Shaper::instance();
        std::u8string _text;
        core::sizef _bounds;
        std::shared_ptr<SkTextBlob> _blob;

        std::vector<uint16_t> _rtf_font_indices;
        std::vector<uint32_t> _rtf_colors;

        // 0 ltr 1 rtl 0xfe auto but preffer ltf 0xff auto but preffer rtl
        uint8_t _defaultBidiLevel = 0xfe;
#ifdef _DEBUG
        std::u32string _u32text;
#endif
        std::vector<item> _items;
        std::vector<glyph> _glyphs;
        std::vector<cluster> _clusters;
        std::vector<segment> _segments;

        glyph _elid;
        uint16_t _font_default = 0;
        core::color _color_default = core::colors::Auto;
        float32_t _ascent = 0.0f;
        float32_t _descent = 0.0f;
    };

    class TextWraper : public Text
    {
    public:
        TextWraper() = default;
        TextWraper(std::u8string text) :Text(text) {}
        ~TextWraper() = default;

        using Text::layout;
        core::error layout(float32_t end, drawing::wrap_mode mode);

    public:
        core::sizef bounds() const;
        std::shared_ptr<SkTextBlob> build();

    public:
        using Text::findCluster;
        const cluster & findCluster(float32_t pos, size_t lindex) const;

    protected:
        std::vector<row> _lines;
    };
}
