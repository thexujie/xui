#include "stdafx.h"
#include "skia.h"
#include <SkFontMgr.h>

namespace drawing
{
    void __fontmetrics(const drawing::font & font, drawing::fontmetrics & metrics)
    {
        drawing::skia::fontmetrics(font, metrics);
    }
}

namespace drawing::skia
{
    SkFontStyle from(const drawing::font_style & style)
    {
        return SkFontStyle{ int(style.weight), int(style.width), SkFontStyle::Slant(style.slant) };
    }

    SkFontStyle fontstyle(font_weight weight, font_width width, font_slant slant)
    {
        return SkFontStyle{ int(weight), int(width), SkFontStyle::Slant(slant) };
    }


    void fontmetrics(const drawing::font & font, drawing::fontmetrics & metrics)
    {
        sk_sp<SkFontMgr> fontMgr = SkFontMgr::RefDefault();
        auto tf =fontMgr->matchFamilyStyle(reinterpret_cast<const char *>(font.family.c_str()), from(font.style));

        SkPaint paint;
        paint.setTypeface(sk_ref_sp(tf));
        paint.setTextSize(font.size);

        SkPaint::FontMetrics skfm;
        paint.getFontMetrics(&skfm);
        //metrics.height = std::abs(std::ceilf(skfm.fAscent)) + std::ceilf(skfm.fDescent);
        //metrics.top = std::abs(skfm.fTop);
        //metrics.bottom = skfm.fBottom;
        //metrics.ascent = std::abs(skfm.fAscent);
        //metrics.descent = skfm.fDescent;
        //metrics.width = skfm.fAvgCharWidth;
        //if (core::equal(metrics.width, 0.0f))
        //    metrics.width = skfm.fXMax;

        metrics.height = std::abs(std::ceilf(skfm.fAscent)) + std::ceilf(skfm.fDescent);
        metrics.top = std::abs(skfm.fTop);
        metrics.bottom = skfm.fBottom;
        metrics.ascent = std::abs(skfm.fAscent);
        metrics.descent = skfm.fDescent;
        metrics.width = skfm.fAvgCharWidth;
        if (core::equal(metrics.width, 0.0f))
            metrics.width = skfm.fXMax;
    }
}
