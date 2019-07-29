#include "stdafx.h"
#include "PathStyle.h"
#include <SkPaint.h>
#include "skia/skia.h"
#include <SkDashPathEffect.h>

namespace drawing
{
    stroke_style parseStrokeStyle(const std::u8string & str)
    {
        for(const auto & name : stroke_style_names)
        {
            if (core::equal_ic(str, name.name, -1))
                return name.style;
        }
        return stroke_style::none;
    }

    core::vec2<stroke_style> parseStrokeStyle2D(const std::u8string & str)
    {
        std::vector<std::u8string> strs = core::split(str, u8' ');
        if (strs.size() == 1)
            return core::vec2<stroke_style>{ parseStrokeStyle(strs[0]) };
        if (strs.size() == 2)
            return { parseStrokeStyle(strs[0]), parseStrokeStyle(strs[1]) };
        return {};
    }

    core::vec4<stroke_style> parseStrokeStyle4D(const std::u8string & str)
    {
        std::vector<std::u8string> strs = core::split(str, u8' ');
        if (strs.size() == 1)
            return core::vec4<stroke_style>{ parseStrokeStyle(strs[0]) };
        if (strs.size() == 2)
            return { parseStrokeStyle(strs[0]), parseStrokeStyle(strs[1]) };
        if (strs.size() == 4)
            return { parseStrokeStyle(strs[0]), parseStrokeStyle(strs[1]), parseStrokeStyle(strs[2]), parseStrokeStyle(strs[3]) };
        return {};
    }

    void PathStyle::apply(SkPaint & paint) const
    {
        paint.setColor(_color);
        paint.setStrokeWidth(_width);
        paint.setStyle(skia::from(_mode));
		paint.setBlendMode(skia::from(_blend_mode));
		paint.setStrokeJoin(skia::from(_join_style));
        if (!std::isnan(_miter))
            paint.setStrokeMiter(_miter);
        if(_mode == PathMode::Stroke)
        {
            if (_stroke_style == stroke_style::dashed)
            {
                SkScalar intervals[] = { _width, _width };
                paint.setPathEffect(SkDashPathEffect::Make(intervals, SK_ARRAY_COUNT(intervals), 0));
            }
        }
        paint.setAntiAlias(_aa);
    }
}
