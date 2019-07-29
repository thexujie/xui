#pragma once

#include <string>
#include <vector>
#include <usp10.h>
#include "ScriptService.h"
#pragma comment(lib, "usp10.lib")

namespace win32::uniscribe
{
    using namespace drawing;

    struct tlrect_t
    {
        int32_t x;
        int32_t y;
        int32_t w;
        int32_t h;
    };

    // range
    struct range_t
    {
        int32_t index = -1;
        int32_t length = 0;

        range_t operator + (const range_t & rhs)
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

        range_t & operator += (const range_t & rhs)
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


    enum paragraph_tag_e
    {
        paragraph_tag_none = 0x00,
        //! L"\r"
        paragraph_tag_cr = 0x11,
        //! L"\n"
        paragraph_tag_lf = 0x21,
        //! L"\r\n"
        paragraph_tag_crlf = 0x32,
        //! L"\u2028"
        paragraph_tag_ls = 0x41,
        //! L"\u2029"
        paragraph_tag_ps = 0x51,

        TextParagraphTagCharCountMask = 0xF,
    };


    // text range
    using trange_t = range_t;

    // glyph range
    using grange_t = range_t;

    // cluster range
    using crange_t = range_t;

    enum wrapmode_e
    {
        wrapmode_none,
        wrapmode_char,
        wrapmode_word,
    };

    // view range
    using vrange_t = range_t;

    struct scp_format
    {
        int32_t font = 0;
        int32_t color = 0;
    };

    // run range
    struct rrange_t
    {
        int32_t index;
        int32_t length;
    };

    struct scp_item
    {
#ifdef _DEBUG
        std::wstring _text;
#endif
        SCRIPT_ANALYSIS sa;
        trange_t trange;
        crange_t crange;
    };

    struct scp_cluster
    {
        enum
        {
            whitespace = 0x0001,
            softbreak = 0x0002,
            paragraphtag = 0x0004,
            softhyphen = 0x0008,
            right2left = 0x0010,
            linetag = 0x0020,
        };

#ifdef _DEBUG
        std::wstring _text;
#endif
        int32_t index = 0;
        int32_t run = 0;
        trange_t trange;
        grange_t grange;
        int32_t advance = 0;

        int32_t x = 0;
        int32_t y = 0;

        int32_t height = 0;

        scp_format format;

        int32_t flags = 0;
    };

    struct scp_run
    {
#ifdef _DEBUG
        std::wstring _text;
#endif
        int32_t index = -1;
        int32_t item = -1;

        SCRIPT_ANALYSIS sa;
        int32_t font;
        trange_t trange;
        crange_t crange;
        grange_t grange;
        rrange_t rrange;
    };

    struct scp_view
    {
#ifdef _DEBUG
        std::wstring _text;
#endif
        int32_t index = -1;
        int32_t line = -1;
        int32_t run = -1;
        int32_t color = -1;
        crange_t crange;
        grange_t grange;

        int32_t width = 0;
        int32_t height = 0;
        int32_t ascent = 0;
        int32_t decent = 0;
    };

    struct scp_line
    {
#ifdef _DEBUG
        std::wstring _text;
#endif
        int32_t line = 0;
        crange_t crange;
        vrange_t vrange;

        int32_t width = 0;
        int32_t height = 0;

        int32_t ascent = 0;
        int32_t decent = 0;
    };


    class ScriptItem 
    {
    public:
        ScriptItem();
        ~ScriptItem();

        void SetText(std::wstring text);
        const std::wstring & Text() { return _text; }

        // generate scripts and clusters(by ScriptBreak).
        core::error Itemize();
        // generate runs for different font(name、size、bold、italic...)
        core::error Slice();
        // generate glyphs for each run, and calculate widths of all clusters and runs.
        core::error Shape();

        // layout all clusters
        void Layout(int32_t start, int32_t width, wrapmode_e wrapmode);

        void Draw(HDC hdc, int32_t x, int32_t y, core::rc32i rect) const;

        void SetFont(int32_t index, int32_t length, const font & font);
        void SetColor(int32_t index, int32_t length, int32_t color);

        int32_t NumClusters() const { return _clusters.size(); }

    public:
        void Clear();
        core::si32i Size() const;

    private:
        std::shared_ptr<ScriptService> _service;

        std::wstring _text;
        HRESULT _hr = S_OK;

        // 被拆分（Itemize）出的语言段，item 内的文字都是同一语言的，一个 item 可被拆分（Slice）成多个 run
        std::vector<scp_item> _items;

        // 被拆分的图形段，cluster 表示一个『字』
        std::vector<scp_cluster> _clusters;

        // 具有相同字体的一段文字，run 内的文字都是同一语言的，多个 run 一起构成一个 item
        std::vector<scp_run> _runs;

        std::vector<scp_view> _views;

        std::vector<scp_line> _lines;

        // -----------------------------显示相关
        // glyph indices
        std::vector<uint16_t> _glyphs;
        std::vector<SCRIPT_VISATTR> _glyph_attrs;
        std::vector<int32_t> _glyph_advances;
        std::vector<GOFFSET> _glyph_offsets;
        std::vector<uint16_t> _cluster_indices;
    };

}