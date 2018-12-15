#pragma once

class SkPaint;

namespace drawing
{
    struct ImageFormat
    {
        ImageFormat() {}

        ImageFormat & blend_mode(drawing::blend_mode b)
        {
			_blend_mode = b;
            return *this;
        }

        void apply(SkPaint & paint) const;

        drawing::blend_mode _blend_mode = drawing::blend_mode::src_over;
    };
}
