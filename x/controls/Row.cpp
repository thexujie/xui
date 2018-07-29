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

    core::si32f Row::contentSize() const
    {
        core::si32f size;
        float32_t margin = 0;
        for (auto & control : _controls)
        {
            auto lo = control->layoutOrigin();
            if (lo != layout_origin::layout && lo != layout_origin::sticky)
                continue;

            auto m = control->margin();
            margin = std::max(margin, m.bleft);
            size.cx += margin;
            auto psize = control->prefferSize();
            margin = m.bright;
            size.cx = size.cx + psize.cx;
            size.cy = std::max(size.cy, psize.cy + m.bheight());
        }
        return size;
    }

    void Row::layout()
    {
        float32_t margin = 0;
        core::si32f size = prefferSize();

        core::rc32f rc;
        rc.pos = paddingBox().leftTop();
        rc.cy = size.cy;
        for (auto & control : _controls)
        {
            auto m = control->margin();
            auto ps = control->prefferSize();
            auto lo = control->layoutOrigin();

            if (lo != layout_origin::layout && lo != layout_origin::sticky)
            {
                switch (lo)
                {
                case layout_origin::parent:
                    control->layout(_rect, ps);
                    break;
                case layout_origin::scene:
                    control->layout(scene()->rect(), ps);
                    break;
                case layout_origin::view:
                    control->layout(scene()->viewRect(), ps);
                    break;
                default:
                    break;
                }
                continue;
            }

            margin = std::max(margin, m.bleft);
            rc.x += margin;
            rc.y = m.btop;
            rc.size = ps;
            control->layout(rc, ps);
            margin = m.bright;
            rc.x += rc.cx;
        }
    }
}
