#pragma once

#include <string>
#include <vector>
#include <usp10.h>
#include "graphics/text/text.h"
#include "ScriptService.h"
#include "graphics/GraphicsService.h"
#pragma comment(lib, "usp10.lib")

namespace win32::uniscribe
{
    using namespace graphics::text;

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

        bool whitespace  = false;
        //! ���ַ����Ƿ��ǽ���Ļ���λ��.
        bool softbreak = false;
        //! �Ƿ��Ƕ�����
        bool paragraphtag = false;
        //! �Ƿ������ַ�
        bool softhyphen = false;
        //! �Ƿ��Ǵ��ҵ�����Ķ�˳��
        bool right2left = false;
        //! �Ƿ����б��
        bool linetag = false;
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
    };


    class ScriptItem : public graphics::IGraphicsString
    {
    public:
        ScriptItem();
        ~ScriptItem();

        void SetText(std::wstring text);
        const std::wstring & Text() { return _text; }

        // generate scripts and clusters(by ScriptBreak).
        core::error_e Itemize();
        // generate runs for different font(name��size��bold��italic...)
        core::error_e Slice();
        // generate glyphs for each run, and calculate widths of all clusters and runs.
        core::error_e Shape();

        // layout all clusters
        void Layout(int32_t start, int32_t width, wrapmode_e wrapmode);

        void Draw(HDC hdc, int32_t x, int32_t y, core::rc32i rect);

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

        // ����֣�Itemize���������ԶΣ�item �ڵ����ֶ���ͬһ���Եģ�һ�� item �ɱ���֣�Slice���ɶ�� run
        std::vector<scp_item> _items;

        // ����ֵ�ͼ�ζΣ�cluster ��ʾһ�����֡�
        std::vector<scp_cluster> _clusters;

        // ������ͬ�����һ�����֣�run �ڵ����ֶ���ͬһ���Եģ���� run һ�𹹳�һ�� item
        std::vector<scp_run> _runs;

        std::vector<scp_view> _views;

        std::vector<scp_line> _lines;

        // -----------------------------��ʾ���
        // glyph indices
        std::vector<uint16_t> _glyphs;
        std::vector<SCRIPT_VISATTR> _glyph_attrs;
        std::vector<int32_t> _glyph_advances;
        std::vector<GOFFSET> _glyph_offsets;
        std::vector<uint16_t> _cluster_indices;
    };

}