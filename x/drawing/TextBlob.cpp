#include "stdafx.h"
#include "TextBlob.h"
#include "Shaper.h"
#include <SkTextBlob.h>
#include "skia/skia.h"

namespace drawing
{
    TextBlob::TextBlob()
    {
    }

    TextBlob::TextBlob(std::string str, const StringFormat & format)
    {
        reset(str, -1, format);
    }

    TextBlob::TextBlob(std::string str, float32_t width, const StringFormat & format)
    {
        reset(str, width, format);
    }

    TextBlob::~TextBlob()
    {
    }

    void TextBlob::reset(std::string str, float32_t width, const StringFormat & format)
    {
        if (width < 0)
            width = std::numeric_limits<float32_t>::max();

        Shaper shaper(format._font, format._color);
        shaper.reset(str);
        shaper.itermize();
        shaper.wrap(width, drawing::wrap_mode::word);

        _size = shaper.lineSize(0);
        _native = shaper.build(0);
    }
}
