#pragma once
#include "core/error.h"

#include <hb-ot.h>
#include <unicode/brkiter.h>
#include <unicode/locid.h>
#include <unicode/stringpiece.h>
#include <unicode/ubidi.h>
#include <unicode/uchriter.h>
#include <unicode/unistr.h>
#include <unicode/uscript.h>

namespace script
{
    // range
    struct range
    {
        int32_t index = -1;
        int32_t length = 0;

        range operator + (const range & rhs)
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

        range & operator += (const range & rhs)
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

    class IShaper
    {
    public:
        virtual ~IShaper() {}

        virtual core::error reset(std::string text) = 0;
        virtual core::error itermize() = 0;
    };

    struct item
    {
        range range;
        hb_script_t script;
        bool rtl;
#ifdef _DEBUG
        std::u16string _text;
#endif
    };

    class Shaper : public IShaper
    {
    public:
        Shaper() = default;

        core::error reset(std::string text);
        core::error itermize();
        core::error clusterize();

    private:
        std::string _text;
        std::vector<item> _items;
    };
}
