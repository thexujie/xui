#include "stdafx.h"
#include "TextBlob.h"
#include "skia/SkShaper.h"
#include <SkTextBlob.h>
#include "skia/skia.h"
#include "skia/script.h"

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

        static int type = 2;
        if(type == 1)
        {
            SkPaint paint;
            format.apply(paint);

            SkShaper shaper(SkTypeface::MakeFromName(format._font.family.c_str(), skia::from(format._font.style)));

            SkTextBlobBuilder builder;
            SkRect rcBlob;
            SkPoint end = shaper.shape(builder, paint, str.c_str(), str.length(), true, {}, width, rcBlob);
            _size = { rcBlob.width(), rcBlob.height() };
            _native = std::shared_ptr<SkTextBlob>(builder.make().release(), [](SkTextBlob * ptr) { if (ptr) SkSafeUnref(ptr); });
        }
        else if (type == 2)
        {
            script::Shaper shaper(format._font, format._color);
            shaper.reset(str);
            shaper.itermize();
            shaper.wrap(width, drawing::wrap_mode::word);

            SkTextBlobBuilder builder;
            shaper.build(builder, 0);
            _size = shaper.lineSize(0);
            _native = std::shared_ptr<SkTextBlob>(builder.make().release(), [](SkTextBlob * ptr) { if (ptr) SkSafeUnref(ptr); });
        }
    }
}
