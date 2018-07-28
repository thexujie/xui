#include "stdafx.h"
#include "Row.h"
#include "renderables/Image.h"
#include "renderables/Rectangle.h"

namespace controls
{
    Row::Row()
    {
        
    }

    Row::~Row()
    {
        
    }

    core::si32f Row::expectContentSize() const
    {
        core::si32f size;
        float32_t margin = 0;
        for (auto & control : _controls)
        {
            auto m = control->margin();
            margin = std::max(margin, m.bleft);
            size.cx += margin;
            auto esize = control->expectSize();
            margin = m.bright;
            size.cx = size.cx + esize.cx;
            size.cy = std::max(size.cy, esize.cy);
        }
        return size;
    }

    void Row::layout(const core::rc32f & rect)
    {
        LayoutState state;
        Container::layout(state, rect);
        if(rect.empty())
        {
            float32_t margin = 0;
            core::si32f size;
            size.clear();
            margin = 0;
            for (auto & control : _controls)
            {
                auto m = control->margin();
                margin = std::max(margin, m.bleft);
                size.cx += margin;
                auto esize = control->expectSize();
                margin = m.bright;
                size.cx = size.cx + esize.cx;
                size.cy = std::max(esize.cy, size.cy);
            }
            size.cx += margin;

            core::rc32f rc;
            rc.y = 0;
            rc.cy = size.cy;
            for (auto & control : _controls)
            {
                auto m = control->margin();
                margin = std::max(margin, m.bleft);
                rc.x += margin;
                rc.cx = control->width();
                control->layout(state, rc);
                margin = m.bright;
                rc.x += rc.cx;
                size.cy = std::max(control->height(), size.cy);
            }

            _view_content_size = size;
        }
    }
}
