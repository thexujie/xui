#include "stdafx.h"
#include "TextBlob.h"
#include "skia/SkShaper.h"
#include <SkTextBlob.h>

namespace graphics
{
    TextBlob::TextBlob()
    {
    }

    TextBlob::TextBlob(std::string str, int32_t width, const StringFormat & format)
    {
        reset(str, width, format);
    }

    TextBlob::~TextBlob()
    {
    }

    void TextBlob::reset(std::string str, int32_t width, const StringFormat & format)
    {
        SkPaint paint;
        format.apply(paint);

        SkShaper shaper(SkTypeface::MakeFromName(format._font.family.c_str(), SkFontStyle(format._font.weight, format._font.width, (SkFontStyle::Slant)format._font.slant)));

        SkTextBlobBuilder builder;
        SkRect rcBlob;
        SkPoint end = shaper.shape(builder, paint, str.c_str(), str.length(), true, {}, width, rcBlob);

        _native = std::shared_ptr<SkTextBlob>(builder.make().release(), [](SkTextBlob * ptr) { if (ptr) SkSafeUnref(ptr); });
    }
}
