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

    void Row::layout(const core::rc32f & rect)
    {
        Container::layout(rect);
        if(rect.empty())
        {
            float32_t margin = 0;
            core::si32f size;
            for (auto & control : _controls)
            {
                auto m = control->margin();
                margin = std::max(margin, m.bleft);
                size.cx += margin;
                control->layout(core::rc32f());
                margin = m.bright;
                size.cx = size.cx + control->width();
                size.cy = std::max(control->height(), size.cy);
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
                control->layout(rc);
                margin = m.bright;
                size.cy = std::max(control->height(), size.cy);
            }

            _view_content_size = size;
        }
    }
}
