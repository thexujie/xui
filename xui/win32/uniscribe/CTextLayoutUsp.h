#pragma once

#include "UspInc.h"
namespace win32
{
    namespace script
    {
        struct usp_line_t
        {
            int32_t line;

            int32_t index;
            int32_t length;

            int32_t cluster_index;
            int32_t cluster_length;

            int32_t x;
            int32_t y;
            int32_t width;
            int32_t height;
        };

        class CTextLayoutUsp
        {
        public:
            CTextLayoutUsp(class CUspFactory * pFactory);
            ~CTextLayoutUsp();

            err_t Initialize(IDocSource * pSrouce);
            IDocSource * GetSource() const;

            err_t Layout(trange_t range, int32_t iWidth, paragraph_tag_e tag = paragraph_tag_none);
            err_t Branch(int32_t iWidth);

            tl_cluster_t FindCluster(int32_t iIndex) const;
            tl_cluster_t GetCluster(int32_t iCluster) const;
            tl_line_t GetLine(int32_t iLine) const;

            tl_range_t HitTest(pointix point) const;
            int32_t HitTestRange(int32_t iIndex, int32_t iLength, tl_range_t * rects, int32_t iCount) const;
            tl_metrics_t GetMetrics() const;
        public:
            winerr_t Draw(HDC hdc, int32_t iX, int32_t iY, uint_32 color, const rectix & rect) const;
            winerr_t DrawRun(HDC hdc, int32_t iX, int32_t iY, const RECT & rect,
                int32_t iRun, int32_t iCluster, int32_t iClusterLength) const;

        private:
            CUspFactory * m_pFactory;

            IDocSource * m_pSource;

            trange_t m_range;
            paragraph_tag_e m_tag;
            sizeix m_size;

            // run
            vector<usp_run_t> m_runs;
            // glyph
            vector<uint_16> m_glyphs;
            // advance
            vector<int_32> m_advances;
            // offset
            vector<GOFFSET> m_offsets;
            // scp cluster
            vector<scp_cluster_t> m_scpclusters;
            // cluster
            vector<usp_cluster_t> m_clusters;
            // line
            vector<tl_line_t> m_lines;
        };
    }
}



