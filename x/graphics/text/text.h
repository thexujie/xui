#pragma once

#include "font.h"
#include "core/unicode.h"

namespace graphics::text
{
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
}
