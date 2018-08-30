#include "stdafx.h"
#include "skia.h"
#include <SkFontMgr.h>

namespace graphics
{
    void __fontmetrics(const graphics::font & font, graphics::fontmetrics & metrics)
    {
        graphics::skia::fontmetrics(font, metrics);
    }
}

namespace graphics::skia
{
    SkFontStyle fontstyle(font_weight weight, font_width width, font_slant slant)
    {
        return SkFontStyle{ int(weight), int(width), SkFontStyle::Slant(slant) };
    }


    void fontmetrics(const graphics::font & font, graphics::fontmetrics & metrics)
    {
        sk_sp<SkFontMgr> fontMgr = SkFontMgr::RefDefault();
        auto tf =fontMgr->legacyMakeTypeface(font.family.c_str(), fontstyle(font.weight, font.width, font.slant));

        SkPaint paint;
        paint.setTypeface(tf);
        paint.setTextSize(font.size);

        SkPaint::FontMetrics skfm;
        paint.getFontMetrics(&skfm);
        metrics.height = std::abs(skfm.fAscent) + skfm.fDescent;
        metrics.top = std::abs(skfm.fTop);
        metrics.bottom = skfm.fBottom;
        metrics.ascent = std::abs(skfm.fAscent);
        metrics.descent = skfm.fDescent;
        metrics.leading = skfm.fLeading;
    }
}
