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

    TextBlob::TextBlob(std::shared_ptr<SkTextBlob> native, core::sizef size) : _native(native), _size(size)
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

        TextWraper shaper(str);
        shaper.itermize(format._font, format._color);
        shaper.layout(width, drawing::wrap_mode::word);

        _size = shaper.bounds();
        _native = shaper.build();
    }
}
