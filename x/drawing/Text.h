#pragma once

class SkFont;
class SkTypeface;
class SkTextBlob;

struct hb_font_t;
struct hb_buffer_t;

namespace icu
{
    class BreakIterator;
}

namespace drawing
{
    using core::span32;
    using core::span8;

    constexpr uint32_t hb_make_tag(char c1, char c2, char c3, char c4)
    {
        return ((uint32_t)((((uint8_t)(c1)) << 24) | (((uint8_t)(c2)) << 16) | (((uint8_t)(c3)) << 8) | ((uint8_t)(c4))));
    }

    constexpr uint32_t HB_TAG_MAKE_NONE = hb_make_tag(0, 0, 0, 0);
    enum class hb_script
    {
        /*1.1*/ common = hb_make_tag('Z', 'y', 'y', 'y'),
        /*1.1*/ inherited = hb_make_tag('Z', 'i', 'n', 'h'),
        /*5.0*/ unknown = hb_make_tag('Z', 'z', 'z', 'z'),

        /*1.1*/ arabic = hb_make_tag('A', 'r', 'a', 'b'),
        /*1.1*/ armenian = hb_make_tag('A', 'r', 'm', 'n'),
        /*1.1*/ bengali = hb_make_tag('B', 'e', 'n', 'g'),
        /*1.1*/ cyrillic = hb_make_tag('C', 'y', 'r', 'l'),
        /*1.1*/ devanagari = hb_make_tag('D', 'e', 'v', 'a'),
        /*1.1*/ georgian = hb_make_tag('G', 'e', 'o', 'r'),
        /*1.1*/ greek = hb_make_tag('G', 'r', 'e', 'k'),
        /*1.1*/ gujarati = hb_make_tag('G', 'u', 'j', 'r'),
        /*1.1*/ gurmukhi = hb_make_tag('G', 'u', 'r', 'u'),
        /*1.1*/ hangul = hb_make_tag('H', 'a', 'n', 'g'),
        /*1.1*/ han = hb_make_tag('H', 'a', 'n', 'i'),
        /*1.1*/ hebrew = hb_make_tag('H', 'e', 'b', 'r'),
        /*1.1*/ hiragana = hb_make_tag('H', 'i', 'r', 'a'),
        /*1.1*/ kannada = hb_make_tag('K', 'n', 'd', 'a'),
        /*1.1*/ katakana = hb_make_tag('K', 'a', 'n', 'a'),
        /*1.1*/ lao = hb_make_tag('L', 'a', 'o', 'o'),
        /*1.1*/ latin = hb_make_tag('L', 'a', 't', 'n'),
        /*1.1*/ malayalam = hb_make_tag('M', 'l', 'y', 'm'),
        /*1.1*/ oriya = hb_make_tag('O', 'r', 'y', 'a'),
        /*1.1*/ tamil = hb_make_tag('T', 'a', 'm', 'l'),
        /*1.1*/ telugu = hb_make_tag('T', 'e', 'l', 'u'),
        /*1.1*/ thai = hb_make_tag('T', 'h', 'a', 'i'),

        /*2.0*/ tibetan = hb_make_tag('T', 'i', 'b', 't'),

        /*3.0*/ bopomofo = hb_make_tag('B', 'o', 'p', 'o'),
        /*3.0*/ braille = hb_make_tag('B', 'r', 'a', 'i'),
        /*3.0*/ canadian_syllabics = hb_make_tag('C', 'a', 'n', 's'),
        /*3.0*/ cherokee = hb_make_tag('C', 'h', 'e', 'r'),
        /*3.0*/ ethiopic = hb_make_tag('E', 't', 'h', 'i'),
        /*3.0*/ khmer = hb_make_tag('K', 'h', 'm', 'r'),
        /*3.0*/ mongolian = hb_make_tag('M', 'o', 'n', 'g'),
        /*3.0*/ myanmar = hb_make_tag('M', 'y', 'm', 'r'),
        /*3.0*/ ogham = hb_make_tag('O', 'g', 'a', 'm'),
        /*3.0*/ runic = hb_make_tag('R', 'u', 'n', 'r'),
        /*3.0*/ sinhala = hb_make_tag('S', 'i', 'n', 'h'),
        /*3.0*/ syriac = hb_make_tag('S', 'y', 'r', 'c'),
        /*3.0*/ thaana = hb_make_tag('T', 'h', 'a', 'a'),
        /*3.0*/ yi = hb_make_tag('Y', 'i', 'i', 'i'),

        /*3.1*/ deseret = hb_make_tag('D', 's', 'r', 't'),
        /*3.1*/ gothic = hb_make_tag('G', 'o', 't', 'h'),
        /*3.1*/ old_italic = hb_make_tag('I', 't', 'a', 'l'),

        /*3.2*/ buhid = hb_make_tag('B', 'u', 'h', 'd'),
        /*3.2*/ hanunoo = hb_make_tag('H', 'a', 'n', 'o'),
        /*3.2*/ tagalog = hb_make_tag('T', 'g', 'l', 'g'),
        /*3.2*/ tagbanwa = hb_make_tag('T', 'a', 'g', 'b'),

        /*4.0*/ cypriot = hb_make_tag('C', 'p', 'r', 't'),
        /*4.0*/ limbu = hb_make_tag('L', 'i', 'm', 'b'),
        /*4.0*/ linear_b = hb_make_tag('L', 'i', 'n', 'b'),
        /*4.0*/ osmanya = hb_make_tag('O', 's', 'm', 'a'),
        /*4.0*/ shavian = hb_make_tag('S', 'h', 'a', 'w'),
        /*4.0*/ tai_le = hb_make_tag('T', 'a', 'l', 'e'),
        /*4.0*/ ugaritic = hb_make_tag('U', 'g', 'a', 'r'),

        /*4.1*/ buginese = hb_make_tag('B', 'u', 'g', 'i'),
        /*4.1*/ coptic = hb_make_tag('C', 'o', 'p', 't'),
        /*4.1*/ glagolitic = hb_make_tag('G', 'l', 'a', 'g'),
        /*4.1*/ kharoshthi = hb_make_tag('K', 'h', 'a', 'r'),
        /*4.1*/ new_tai_lue = hb_make_tag('T', 'a', 'l', 'u'),
        /*4.1*/ old_persian = hb_make_tag('X', 'p', 'e', 'o'),
        /*4.1*/ syloti_nagri = hb_make_tag('S', 'y', 'l', 'o'),
        /*4.1*/ tifinagh = hb_make_tag('T', 'f', 'n', 'g'),

        /*5.0*/ balinese = hb_make_tag('B', 'a', 'l', 'i'),
        /*5.0*/ cuneiform = hb_make_tag('X', 's', 'u', 'x'),
        /*5.0*/ nko = hb_make_tag('N', 'k', 'o', 'o'),
        /*5.0*/ phags_pa = hb_make_tag('P', 'h', 'a', 'g'),
        /*5.0*/ phoenician = hb_make_tag('P', 'h', 'n', 'x'),

        /*5.1*/ carian = hb_make_tag('C', 'a', 'r', 'i'),
        /*5.1*/ cham = hb_make_tag('C', 'h', 'a', 'm'),
        /*5.1*/ kayah_li = hb_make_tag('K', 'a', 'l', 'i'),
        /*5.1*/ lepcha = hb_make_tag('L', 'e', 'p', 'c'),
        /*5.1*/ lycian = hb_make_tag('L', 'y', 'c', 'i'),
        /*5.1*/ lydian = hb_make_tag('L', 'y', 'd', 'i'),
        /*5.1*/ ol_chiki = hb_make_tag('O', 'l', 'c', 'k'),
        /*5.1*/ rejang = hb_make_tag('R', 'j', 'n', 'g'),
        /*5.1*/ saurashtra = hb_make_tag('S', 'a', 'u', 'r'),
        /*5.1*/ sundanese = hb_make_tag('S', 'u', 'n', 'd'),
        /*5.1*/ vai = hb_make_tag('V', 'a', 'i', 'i'),

        /*5.2*/ avestan = hb_make_tag('A', 'v', 's', 't'),
        /*5.2*/ bamum = hb_make_tag('B', 'a', 'm', 'u'),
        /*5.2*/ egyptian_hieroglyphs = hb_make_tag('E', 'g', 'y', 'p'),
        /*5.2*/ imperial_aramaic = hb_make_tag('A', 'r', 'm', 'i'),
        /*5.2*/ inscriptional_pahlavi = hb_make_tag('P', 'h', 'l', 'i'),
        /*5.2*/ inscriptional_parthian = hb_make_tag('P', 'r', 't', 'i'),
        /*5.2*/ javanese = hb_make_tag('J', 'a', 'v', 'a'),
        /*5.2*/ kaithi = hb_make_tag('K', 't', 'h', 'i'),
        /*5.2*/ lisu = hb_make_tag('L', 'i', 's', 'u'),
        /*5.2*/ meetei_mayek = hb_make_tag('M', 't', 'e', 'i'),
        /*5.2*/ old_south_arabian = hb_make_tag('S', 'a', 'r', 'b'),
        /*5.2*/ old_turkic = hb_make_tag('O', 'r', 'k', 'h'),
        /*5.2*/ samaritan = hb_make_tag('S', 'a', 'm', 'r'),
        /*5.2*/ tai_tham = hb_make_tag('L', 'a', 'n', 'a'),
        /*5.2*/ tai_viet = hb_make_tag('T', 'a', 'v', 't'),

        /*6.0*/ batak = hb_make_tag('B', 'a', 't', 'k'),
        /*6.0*/ brahmi = hb_make_tag('B', 'r', 'a', 'h'),
        /*6.0*/ mandaic = hb_make_tag('M', 'a', 'n', 'd'),

        /*6.1*/ chakma = hb_make_tag('C', 'a', 'k', 'm'),
        /*6.1*/ meroitic_cursive = hb_make_tag('M', 'e', 'r', 'c'),
        /*6.1*/ meroitic_hieroglyphs = hb_make_tag('M', 'e', 'r', 'o'),
        /*6.1*/ miao = hb_make_tag('P', 'l', 'r', 'd'),
        /*6.1*/ sharada = hb_make_tag('S', 'h', 'r', 'd'),
        /*6.1*/ sora_sompeng = hb_make_tag('S', 'o', 'r', 'a'),
        /*6.1*/ takri = hb_make_tag('T', 'a', 'k', 'r'),

        /*
        * since: 0.9.30
        */
        /*7.0*/ bassa_vah = hb_make_tag('B', 'a', 's', 's'),
        /*7.0*/ caucasian_albanian = hb_make_tag('A', 'g', 'h', 'b'),
        /*7.0*/ duployan = hb_make_tag('D', 'u', 'p', 'l'),
        /*7.0*/ elbasan = hb_make_tag('E', 'l', 'b', 'a'),
        /*7.0*/ grantha = hb_make_tag('G', 'r', 'a', 'n'),
        /*7.0*/ khojki = hb_make_tag('K', 'h', 'o', 'j'),
        /*7.0*/ khudawadi = hb_make_tag('S', 'i', 'n', 'd'),
        /*7.0*/ linear_a = hb_make_tag('L', 'i', 'n', 'a'),
        /*7.0*/ mahajani = hb_make_tag('M', 'a', 'h', 'j'),
        /*7.0*/ manichaean = hb_make_tag('M', 'a', 'n', 'i'),
        /*7.0*/ mende_kikakui = hb_make_tag('M', 'e', 'n', 'd'),
        /*7.0*/ modi = hb_make_tag('M', 'o', 'd', 'i'),
        /*7.0*/ mro = hb_make_tag('M', 'r', 'o', 'o'),
        /*7.0*/ nabataean = hb_make_tag('N', 'b', 'a', 't'),
        /*7.0*/ old_north_arabian = hb_make_tag('N', 'a', 'r', 'b'),
        /*7.0*/ old_permic = hb_make_tag('P', 'e', 'r', 'm'),
        /*7.0*/ pahawh_hmong = hb_make_tag('H', 'm', 'n', 'g'),
        /*7.0*/ palmyrene = hb_make_tag('P', 'a', 'l', 'm'),
        /*7.0*/ pau_cin_hau = hb_make_tag('P', 'a', 'u', 'c'),
        /*7.0*/ psalter_pahlavi = hb_make_tag('P', 'h', 'l', 'p'),
        /*7.0*/ siddham = hb_make_tag('S', 'i', 'd', 'd'),
        /*7.0*/ tirhuta = hb_make_tag('T', 'i', 'r', 'h'),
        /*7.0*/ warang_citi = hb_make_tag('W', 'a', 'r', 'a'),

        /*8.0*/ ahom = hb_make_tag('A', 'h', 'o', 'm'),
        /*8.0*/ anatolian_hieroglyphs = hb_make_tag('H', 'l', 'u', 'w'),
        /*8.0*/ hatran = hb_make_tag('H', 'a', 't', 'r'),
        /*8.0*/ multani = hb_make_tag('M', 'u', 'l', 't'),
        /*8.0*/ old_hungarian = hb_make_tag('H', 'u', 'n', 'g'),
        /*8.0*/ signwriting = hb_make_tag('S', 'g', 'n', 'w'),

        /*
        * since 1.3.0
        */
        /*9.0*/ adlam = hb_make_tag('A', 'd', 'l', 'm'),
        /*9.0*/ bhaiksuki = hb_make_tag('B', 'h', 'k', 's'),
        /*9.0*/ marchen = hb_make_tag('M', 'a', 'r', 'c'),
        /*9.0*/ osage = hb_make_tag('O', 's', 'g', 'e'),
        /*9.0*/ tangut = hb_make_tag('T', 'a', 'n', 'g'),
        /*9.0*/ newa = hb_make_tag('N', 'e', 'w', 'a'),

        /*
        * since 1.6.0
        */
        /*10.0*/masaram_gondi = hb_make_tag('G', 'o', 'n', 'm'),
        /*10.0*/nushu = hb_make_tag('N', 's', 'h', 'u'),
        /*10.0*/soyombo = hb_make_tag('S', 'o', 'y', 'o'),
        /*10.0*/zanabazar_square = hb_make_tag('Z', 'a', 'n', 'b'),

        /*
        * since 1.8.0
        */
        /*11.0*/dogra = hb_make_tag('D', 'o', 'g', 'r'),
        /*11.0*/gunjala_gondi = hb_make_tag('G', 'o', 'n', 'g'),
        /*11.0*/hanifi_rohingya = hb_make_tag('R', 'o', 'h', 'g'),
        /*11.0*/makasar = hb_make_tag('M', 'a', 'k', 'a'),
        /*11.0*/medefaidrin = hb_make_tag('M', 'e', 'd', 'f'),
        /*11.0*/old_sogdian = hb_make_tag('S', 'o', 'g', 'o'),
        /*11.0*/sogdian = hb_make_tag('S', 'o', 'g', 'd'),

        /* no script set. */
        invalid = hb_make_tag(0, 0, 0, 0),
    };

    enum class bidirection
    {
        ltr = 0,
        rtl,
    };

    struct item
    {
        uint16_t iindex = 0;
        span32 trange;
        span32 grange;
        span32 crange;
        // hb_script_t
        hb_script script = hb_script::invalid;
        uint8_t level = 0;
        bidirection bidi = bidirection::ltr;
        uint16_t font = 0;
        uint32_t color = 0;
        core::vec2f advance;
#ifdef _DEBUG
        std::u8string _text;
        drawing::font _font;
#endif
    };

    struct glyph
    {
        uint16_t gindex = 0;
        uint16_t cindex = 0;
        uint16_t sindex = 0;
        char32_t codepoint = 0;

        bidirection bidi = bidirection::ltr;
        uint16_t gid = 0;
        core::vec2f advance;
        core::vec2f offset;
        bool standalone = false;
    };

    struct cluster
    {
        uint16_t cindex = 0;
        uint16_t iindex = 0;
        uint16_t sindex = 0;
        span32 trange;
        span32 grange;
        core::vec2f advance;
        core::vec4f rect;
        bidirection bidi = bidirection::ltr;
        bool wordbreak = false;

        operator bool() const { return trange.length > 0; }
#ifdef _DEBUG
        std::u8string _text;
#endif
    };

    struct segment
    {
        uint32_t sindex = 0;
        uint32_t lindex = 0;
        uint32_t iindex = 0;
        span32 trange;
        span32 grange;
        span32 crange;
        core::vec2f advance;

        bidirection bidi = bidirection::ltr;
        float32_t offset = 0;
#ifdef _DEBUG
        std::u8string _text;
#endif
    };

    struct row
    {
        span32 trange;
        span32 crange;
        span32 srange;
        uint32_t line = 0;
        float32_t width = 0;
        float32_t ascent = 0;
        float32_t descent = 0;
#ifdef _DEBUG
        std::u8string _text;
#endif
    };

    constexpr size_t cursor_pos_mask = size_t(-1) >> 1;
    constexpr size_t cursor_pos_far = ~cursor_pos_mask;
    constexpr size_t cursor_pos_nopos = cursor_pos_far | cursor_pos_mask;

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

        std::shared_ptr<class SkTextBlobBuilder> builder();
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

        std::shared_ptr<class SkTextBlobBuilder> _builder;
        std::unique_ptr<icu::BreakIterator, void(*)(icu::BreakIterator *)> _breaker_world = { nullptr, nullptr };
        std::unique_ptr<icu::BreakIterator, void(*)(icu::BreakIterator *)> _breaker_character = { nullptr, nullptr };


    public:
        static inline glyph empty_glyph = {};
        static inline cluster empty_cluster = {};
    };

    class Text
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
        core::error update(const drawing::font & font, core::color color);

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
        core::error layout(float32_t end, wrap_mode mode);

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
