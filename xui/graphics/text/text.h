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
            // �ı���Χ
            trange_t trange;
            // ������
            paragraph_tag_e tag;
        };

        /**
        * ��ʾһ����ʾ�ڲ����еĶ�����Ԫ��һ�����߸���� char_16 һ���ʾһ�� cluster��
        */
        struct tl_cluster_t
        {
            // cluster ����
            int32_t cluster;
            // �к�
            int32_t line;
            // ����
            int32_t para;
            // �ı���Χ
            trange_t trange;
            // cluster �ڲ����е�λ��
            tlrect_t tlrect;

            bool whitespace : 1;
            //! �Ƿ��ֹ�ڸ��ַ�������
            bool softbreak : 1;
            //! �Ƿ��Ƕ�����
            bool paragraphtag : 1;
            //! �Ƿ������ַ�
            bool softhyphen : 1;
            //! �Ƿ��Ǵ��ҵ�����Ķ�˳��
            bool right2left : 1;
            //! �Ƿ����б��
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

        // ��ʾ���η�Χ
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