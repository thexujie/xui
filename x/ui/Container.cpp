#include "stdafx.h"
#include "Container.h"
#include "controls/ScrollBar.h"

namespace ui
{
    Container::Container()
    {
        _interactable = false;
    }

    Container::~Container()
    {
        
    }

    void Container::addControl(std::shared_ptr<Control> control)
    {
        std::lock_guard lock(*this);
        if (std::any_of(_controls.begin(), _controls.end(), [&control](const auto & pair) { return pair.second == control; }))
            return;

        auto s = scene();
        control->setParent(share_ref<Container>());
        if(s)
        {
            control->enteringScene(s);
            _controls.insert(std::make_pair(control->ZValue(), control));
            control->enterScene(s);
        }
        else
            _controls.insert(std::make_pair(control->ZValue(), control));
        relayout();
    }

    void Container::removeControl(std::shared_ptr<Control> control)
    {
        std::lock_guard lock(*this);
        control->leavingScene();
        for (auto iter = _controls.begin(); iter != _controls.end(); )
        {
            if (iter->second == control)
            {
                control->setParent(nullptr);
                _controls.erase(iter);
                break;
            }
        }
        control->leaveScene();
    }

    void Container::enteringScene(std::shared_ptr<Scene> & scene)
    {
        Control::enteringScene(scene);
        for (auto & iter : _controls)
            iter.second->enteringScene(scene);
    }

    void Container::enterScene(std::shared_ptr<Scene> & scene)
    {
        Control::enterScene(scene);
        for (auto & iter : _controls)
            iter.second->enterScene(scene);
    }

    void Container::leavingScene()
    {
        for (auto & iter : _controls)
            iter.second->leavingScene();
        Control::leavingScene();
    }

    void Container::leaveScene()
    {
        for (auto & iter : _controls)
            iter.second->leaveScene();
        Control::leaveScene();
    }

    void Container::update()
    {
        Control::update();
        for (auto & iter : _controls)
            iter.second->update();
    }

    int32_t Container::animate()
    {
        int32_t num = Control::animate();
        for (auto & iter : _controls)
            num += iter.second->animate();
        return num;
    }

    void Container::ondraw(drawing::Graphics & graphics, const drawing::Region & region) const
    {
        uint32_t a = std::clamp< uint32_t>(uint32_t(std::round(_alpha * 0xff)), 0, 0xff);
        std::lock_guard lock(*this);        
        if (a != 0xff)
            graphics.saveLayer(box(), a);
        _drawBackground(graphics);
        for (auto & iter : _controls)
        {
            auto rect = iter.second->realRect();
            if(region.intersects(rect.ceil<int32_t>()))
                iter.second->ondraw(graphics, region);
        }
        _drawBorder(graphics);
        if (a != 0xff)
            graphics.restore();
    }

    std::shared_ptr<Control> Container::findChild(const core::pt32f & pos, std::shared_ptr<Control> last) const
    {
        if (_clip_clild && !_rect.contains(pos))
            return nullptr;

        bool found = false;

        std::shared_ptr<Control> control = nullptr;
        for (auto & iter : core::reverse(_controls))
        {
            if (last && !found)
            {
                if (iter.second == last)
                    found = true;
                continue;
            }

            auto hittest = iter.second->hitTest(pos);
            if (hittest == hittest_result::nowhere)
                continue;

            if (hittest == hittest_result::client)
                return iter.second;

            auto child = iter.second->findChild(pos, last);
            if (child)
                return child;

            if (hittest == hittest_result::stable)
                return iter.second;

            control = iter.second;
        }
        return control;
    }

    void Container::onLayoutedSizeChaged(const core::si32f & from, const core::si32f & to)
    {
        layoutedSizeChaged(from, to);
    }

    void Container::setLayoutedSize(const core::si32f & layouted_size)
    {
        if (layouted_size != _layouted_size)
        {
            _layouted_size = layouted_size;
        }
    }


    void Container::onPosChanged(const core::pt32f & from, const core::pt32f & to)
    {
        relayout();
        Control::onPosChanged(from, to);
    }

    void Container::onSizeChanged(const core::si32f & from, const core::si32f & to)
    {
        layout_flags flags = nullptr;
        if (!core::equal(from.cx, to.cx))
            flags |= layout_flag::resize_cx;
        if (!core::equal(from.cy, to.cy))
            flags |= layout_flag::resize_cy;
        relayout(flags);
        Control::onSizeChanged(from, to);
    }

    void Container::relayout(layout_flags flags)
    {
        _invalid_layout_flags |= flags;
        if (!_invalid_layout)
        {
            _invalid_layout = true;
            invoke([this]()
            {
                layout(_invalid_layout_flags);
                _invalid_layout = false;
                _invalid_layout_flags = nullptr;
            });
        }
    }

    void Container::setLayoutDirection(core::align layout)
    {
        _layout_direction = layout;
    }

    void Container::setScrollbarVisionV(scrollbar_vision scrollbar_vision)
    {
        if (_scrollbar_vision_v != scrollbar_vision)
        {
            _scrollbar_vision_v = scrollbar_vision;
            if (_scrollbar_vision_v == scrollbar_vision::always)
            {
                if (!_scrollbar_v)
                {
                    _scrollbar_v = std::make_shared<controls::ScrollBar>();
                    _scrollbar_v->setSize({ 0.5_em, 100_per });
                    _scrollbar_v->setAnchorBorders(core::align::right | core::align::topBottom);
                    _scrollbar_v->setLayoutOrigin(layout_origin::parent);
                    _scrollbar_v->setZValue(ZVALUE_SCROLLBAR);
                    _scrollbar_v->valueChagned += std::weak_bind(&Container::onScrollValueChangedV, share_ref<Container>(), std::placeholders::_1, std::placeholders::_2);
                }
                addControl(_scrollbar_v);
            }
        }
    }

    void Container::setScrollbarVisionH(scrollbar_vision scrollbar_vision)
    {
        if (_scrollbar_vision_h != scrollbar_vision)
        {
            _scrollbar_vision_h = scrollbar_vision;
            if (_scrollbar_vision_h == scrollbar_vision::always)
            {
                if (!_scrollbar_h)
                {
                    _scrollbar_h = std::make_shared<controls::ScrollBar>();
                    _scrollbar_h->setSize({ 100_per, 0.5_em });
                    _scrollbar_h->setAnchorBorders(core::align::bottom | core::align::leftRight);
                    _scrollbar_h->setLayoutOrigin(layout_origin::parent);
                    _scrollbar_h->setZValue(ZVALUE_SCROLLBAR);
                    _scrollbar_h->setDirection(core::align::left);
                    _scrollbar_h->valueChagned += std::weak_bind(&Container::onScrollValueChangedH, share_ref<Container>(), std::placeholders::_1, std::placeholders::_2);
                }
                addControl(_scrollbar_h);
            }
        }
    }

    void Container::onScrollValueChangedV(float32_t from, float32_t to)
    {
        relayout();
    }

    void Container::onScrollValueChangedH(float32_t from, float32_t to)
    {
        relayout();
    }


    core::si32f Container::contentSize() const
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
                auto perffer_size = control->prefferSize();
                margin = _layout_direction == core::align::left ? m.bright : m.bleft;
                size.cx = size.cx + perffer_size.cx;
                size.cy = std::max(size.cy, perffer_size.cy + m.bheight());
            }
            else if (_layout_direction == core::align::top || _layout_direction == core::align::bottom)
            {
                margin = std::max(margin, _layout_direction == core::align::top ? m.btop : m.bbottom);
                size.cy += margin;
                auto perffer_size = control->prefferSize();
                margin = _layout_direction == core::align::top ? m.bbottom : m.btop;
                size.cy += perffer_size.cy;
                size.cx = std::max(size.cx, perffer_size.cx + m.bwidth());
            }
            else {}
        }
        return size;
    }

    void Container::layout(layout_flags flags)
    {
        if (!_invalid_layout && !flags.any(layout_flag::force))
            return;

        if (!scene())
            return;

        float32_t margin = 0;
        core::rc32f box = paddingBox();
        core::si32f spacing = box.size;
        float32_t layout_pos = 0;
        core::si32f layout_size;
        switch (_layout_direction)
        {
        case core::align::left:
            layout_pos = _rect.x;
            if (_scrollbar_h)
                layout_pos -= _scrollbar_h->value();
            break;
        case core::align::top:
            layout_pos = _rect.y;
            if (_scrollbar_v)
                layout_pos -= _scrollbar_v->value();
            break;
        default:
            break;
        }

        float32_t margin_size = 0;
        float32_t fixed_size = 0;
        for (auto & iter : _controls)
        {
            auto & control = iter.second;
            auto lo = control->layoutOrigin();
            if (lo != layout_origin::layout && lo != layout_origin::sticky)
                continue;

            auto m = control->realMargin();
            auto s = control->size();

            if (_layout_direction == core::align::left || _layout_direction == core::align::right)
            {
                margin = std::max(margin, _layout_direction == core::align::left ? m.bleft : m.bright);
                if (s.cx.unit != core::unit::per && s.cx.avi())
                    fixed_size += calc(s.cx);
            }
            else if (_layout_direction == core::align::top || _layout_direction == core::align::bottom)
            {
                margin = std::max(margin, _layout_direction == core::align::top ? m.btop : m.bbottom);
                if (s.cy.unit != core::unit::per && s.cy.avi())
                    fixed_size += calc(s.cy);
            }
            else {}
            margin_size += margin;
        }

        if (_layout_direction == core::align::left || _layout_direction == core::align::right)
        {
            spacing.cx -= margin_size;
            spacing.cx -= fixed_size;
        }
        else if (_layout_direction == core::align::top || _layout_direction == core::align::bottom)
        {
            spacing.cy -= margin_size;
            spacing.cy -= fixed_size;
        }
        else {}

        std::set<std::string> setes;
        for (auto & iter : _controls)
        {
            auto& control = iter.second;
            auto margins = control->realMargin();
            auto lo = control->layoutOrigin();

            if (lo == layout_origin::layout || lo == layout_origin::sticky)
            {
                switch (_layout_direction)
                {
                case core::align::left:
                {
                    auto preffer_size = control->prefferSize({ spacing.cx, spacing.cy - margins.bheight() });
                    margin = std::max(margin, margins.bleft);
                    control->place({ layout_pos + layout_size.cx + margin, box.y + margins.btop, preffer_size.cx, box.cy - margins.bheight() }, preffer_size);
                    layout_size.cx += margin + preffer_size.cx;
                    margin = margins.bright;
                    break;
                }
                case core::align::top:
                {
                    auto preffer_size = control->prefferSize({ spacing.cx - margins.bwidth(), spacing.cy });
                    margin = std::max(margin, margins.btop);
                    control->place({ box.x + margins.bleft, layout_pos + layout_size.cy + margin, box.cx - margins.bwidth(), preffer_size.cy }, preffer_size);
                    layout_size.cy += margin + preffer_size.cy;
                    margin = margins.bbottom;
                    break;
                }
                default:
                    break;
                }
            }
            else
            {
                auto preffer_size = control->prefferSize(box.size);
                switch (lo)
                {
                case layout_origin::parent:
                    control->place(contentBox(), preffer_size);
                    break;
                case layout_origin::scene:
                    control->place(scene()->rect(), preffer_size);
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

        switch (_layout_direction)
        {
        case core::align::left:
            if (_scrollbar_h)
            {
                _scrollbar_h->setPageValue(contentBox().cx);
                _scrollbar_h->setValues(_scrollbar_h->value(), 0, _layouted_size.cx - contentBox().cx);
            }
            break;
        case core::align::top:
            if (_scrollbar_v)
            {
                _scrollbar_v->setPageValue(contentBox().cy);
                _scrollbar_v->setValues(_scrollbar_v->value(), 0, _layouted_size.cy - contentBox().cy);
            }
            break;
        default:
            break;
        }
    }
}

