#pragma once

#include "font.h"

namespace graphics
{
    namespace text
    {
        struct tlrect_t
        {
            int32_t x;
            int32_t y;
            int32_t w;
            int32_t h;
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

        // range
        struct tlrange_t
        {
            int32_t index;
            int32_t length;
        };

        // text range
        struct trange_t
        {
            int32_t index;
            int32_t length;
        };

        // glyph range
        struct grange_t
        {
            int32_t index;
            int32_t length;
        };

        // cluster range
        struct crange_t
        {
            int32_t index;
            int32_t length;
        };

        // line range
        struct lrange_t
        {
            int32_t index;
            int32_t length;
        };

        struct doc_paragraph_t
        {
            // 文本范围
            trange_t trange;
            // 段落标记
            paragraph_tag_e tag;
        };

        /**
        * 表示一个显示在布局中的独立单元，一个或者更多个 char_16 一起表示一个 cluster。
        */
        struct tl_cluster_t
        {
            // cluster 索引
            int32_t cluster;
            // 行号
            int32_t line;
            // 段落
            int32_t para;
            // 文本范围
            trange_t trange;
            // cluster 在布局中的位置
            tlrect_t tlrect;

            bool whitespace : 1;
            //! 是否禁止在该字符处换行
            bool softbreak : 1;
            //! 是否是段落标记
            bool paragraphtag : 1;
            //! 是否是连字符
            bool softhyphen : 1;
            //! 是否是从右到左的阅读顺序
            bool right2left : 1;
            //! 是否是行标记
            bool linetag : 1;
        };

        struct tl_line_t
        {
            int32_t line;
            int32_t para;
            trange_t trange;
            crange_t crange;
            tlrect_t tlrect;
        };

        struct tl_para_t
        {
            int32_t para;
            trange_t trange;
            crange_t crange;
            lrange_t lrange;
            tlrect_t tlrect;
        };

        // 表示矩形范围
        struct tl_range_t
        {
            int32_t line;
            int32_t para;
            trange_t trange;
            crange_t crange;
            tlrect_t tlrect;
        };

        struct tl_metrics_t
        {
            int32_t length;

            int32_t tl_x;
            int32_t tl_y;
            int32_t tl_width;
            int32_t tl_height;

            int32_t cluster_length;
            int32_t line_length;
        };
    }
}