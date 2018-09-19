#include "stdafx.h"
#include "Grid.h"
#include "controls/ScrollBar.h"

namespace ui
{
    Grid::Grid()
    {
        _interactable = false;
    }

    Grid::~Grid()
    {

    }

    void Grid::setLayoutDirection(core::align layout)
    {
        _layout_direction = layout;
    }

    core::si32f Grid::contentSize() const
    {
        core::si32f size;
        float32_t margin = 0;
        for (auto & iter : _controls)
        {
            auto & control = iter.second;
            auto lo = control->layoutOrigin();
            if (lo != layout_origin::layout && lo != layout_origin::sticky)
                continue;

            auto m = control->realMargin();

            if (_layout_direction == core::align::left || _layout_direction == core::align::right)
            {
                margin = std::max(margin, _layout_direction == core::align::left ? m.bleft : m.bright);
                size.cx += margin;
                auto psize = control->expectSize();
                margin = _layout_direction == core::align::left ? m.bright : m.bleft;
                size.cx = size.cx + psize.cx;
                size.cy = std::max(size.cy, psize.cy + m.bheight());
            }
            else if (_layout_direction == core::align::top || _layout_direction == core::align::bottom)
            {
                margin = std::max(margin, _layout_direction == core::align::top ? m.btop : m.bbottom);
                size.cy += margin;
                auto psize = control->expectSize();
                margin = _layout_direction == core::align::top ? m.bbottom : m.btop;
                size.cy += psize.cy;
                size.cx = std::max(size.cx, psize.cx + m.bwidth());
            }
            else {}
        }
        return size;
    }

    void Grid::layout(layout_flags flags)
    {
        if (!_invalid_layout && !flags.any(layout_flag::force))
            return;

        float32_t margin = 0;
        core::si32f fitting_size = expectSize();
        core::rc32f box = paddingBox();
        float32_t layout_pos = 0;
        core::si32f layout_size;
        switch (_layout_direction)
        {
        case core::align::left:
            layout_pos = _rect.x;
            break;
        case core::align::top:
            layout_pos = _rect.y;
            break;
        default:
            break;
        }
        std::set<std::string> setes;
        for (auto & iter : _controls)
        {
            auto& control = iter.second;
            auto margins = control->realMargin();
            auto lo = control->layoutOrigin();
            auto control_size = control->prefferSize(calc_flag::none);

            if (lo == layout_origin::layout || lo == layout_origin::sticky)
            {
                switch (_layout_direction)
                {
                case core::align::left:
                    margin = std::max(margin, margins.bleft);
                    control->place({ layout_pos + layout_size.cx + margin, box.y + margins.btop, control_size.cx, fitting_size.cy - margins.bheight() }, control_size);
                    layout_size.cx += margin + control_size.cx;
                    margin = margins.bright;
                    break;
                case core::align::top:
                    margin = std::max(margin, margins.btop);
                    control->place({ box.x + margins.bleft, layout_pos + layout_size.cy + margin, fitting_size.cx - margins.bwidth(), control_size.cy }, control_size);
                    layout_size.cy += margin + control_size.cy;
                    margin = margins.bbottom;
                    break;
                default:
                    break;
                }
            }
            else
            {
                switch (lo)
                {
                case layout_origin::parent:
                    control->place(contentBox(), control_size);
                    break;
                case layout_origin::scene:
                    control->place(scene()->rect(), control_size);
                    break;
                default:
                    break;
                }
            }
        }

        if (_layout_direction == core::align::left)
        {
            layout_size.cx += margin;
        }
        else if (_layout_direction == core::align::top)
        {
            layout_size.cy += margin;
        }
        setLayoutedSize(layout_size);
        _invalid_layout = false;
    }

    void Grid::onPosChanged(const core::pt32f & from, const core::pt32f & to)
    {
        relayout();
        Control::onPosChanged(from, to);
    }

    void Grid::onSizeChanged(const core::si32f & from, const core::si32f & to)
    {
        relayout();
        Control::onSizeChanged(from, to);
    }

    void Grid::relayout()
    {
        if (!_invalid_layout)
        {
            _invalid_layout = true;
            invoke([this]() {layout(nullptr); });
        }
    }

    void Grid::onScrollValueChangedV(float32_t from, float32_t to)
    {
        relayout();
    }

    void Grid::onScrollValueChangedH(float32_t from, float32_t to)
    {
        relayout();
    }
}

