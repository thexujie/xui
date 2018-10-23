#include "stdafx.h"
#include "Container.h"
#include "controls/ScrollBar.h"
#include "controls/Spacer.h"

namespace ui
{
    Container::Container()
    {
        _interactable = false;
        _accept_wheel_v = true;
    }

    Container::Container(core::align layout) : _layout_direction(layout)
    {
        _interactable = false;
        _accept_wheel_v = true;
    }

    Container::~Container()
    {
        
    }

    void Container::addControl(std::shared_ptr<Control> control)
    {
        std::lock_guard lock(*this);
        if (std::find(_controls.begin(), _controls.end(), control) != _controls.end())
            return;

        auto s = scene();
        control->setParent(share_ref<Container>());
        if(s)
        {
            control->onEnteringScene(s);
            _controls.push_back(control);
            control->onEnterScene(s);
        }
        else
            _controls.push_back(control);
        relayout();
    }

    std::shared_ptr<controls::Spacer> Container::addSpacer(core::dimensionf size)
    {
        auto spacer = std::make_shared<controls::Spacer>();
        spacer->setSize({ size , size });
        addControl(spacer);
        return spacer;
    }

    void Container::removeControl(std::shared_ptr<Control> control)
    {
        std::lock_guard lock(*this);
        control->onLeavingScene();
        auto iter = std::find(_controls.begin(), _controls.end(), control);
        if (iter != _controls.end())
            _controls.erase(iter);
        control->onLeaveScene();
    }

	void Container::clearControls()
    {
		std::lock_guard lock(*this);
		for(auto & control : _controls)
			control->onLeavingScene();
		auto controls = std::move(_controls);
		for(auto & control : _controls)
			control->onLeaveScene();
    }

    void Container::onEnteringScene(std::shared_ptr<Scene> & scene)
    {
        Control::onEnteringScene(scene);
        for (auto & control : _controls)
            control->onEnteringScene(scene);
    }

    void Container::onEnterScene(std::shared_ptr<Scene> & scene)
    {
        Control::onEnterScene(scene);
        relayout();
        for (auto & control : _controls)
            control->onEnterScene(scene);
    }

    void Container::onLeavingScene()
    {
        for (auto & control : _controls)
            control->onLeavingScene();
        Control::onLeavingScene();
    }

    void Container::onLeaveScene()
    {
        for (auto & control : _controls)
            control->onLeaveScene();
        Control::onLeaveScene();
    }

    void Container::update()
    {
        Control::update();
        for (auto & control : _controls)
            control->update();
    }

    void Container::invalidate(const core::rc32f & rect)
    {
        if(auto p = parent())
			p->invalidate(rect);
    }

	bool Container::updateCompleted() const
    {
		if(_delay_update || _invalid_layout)
			return false;

		for(auto & control : _controls)
		{
			if(!control->updateCompleted())
				return false;
		}
		return true;
    }

    int32_t Container::animate()
    {
        int32_t num = Control::animate();
        for (auto & control : _controls)
            num += control->animate();
        return num;
    }

    core::aligns Container::wheelFreedom() const
    {
        return (_scrollbar_v || _scrollbar_h) ? core::align::mask : core::align::none;
    }

    void Container::ondraw(drawing::Graphics & graphics, const core::rc32f & clip) const
    {
        uint32_t a = std::clamp< uint32_t>(uint32_t(std::round(_alpha * 0xff)), 0, 0xff);
        std::lock_guard lock(*this);
        if (a != 0xff)
            graphics.saveLayer(box(), a);
        //else
            //graphics.saveLayer(box(), a);
        //graphics.setClipRect(_rect);
        _drawBackground(graphics);
        for (auto & control : _controls)
        {
			if(!control->aviliable())
				continue;

            auto rect = control->realRect();
            if(clip.intersect_with(rect))
                control->ondraw(graphics, clip);
        }
        _drawBorder(graphics);
        if (a != 0xff)
        graphics.restore();
    }

    std::shared_ptr<Control> Container::findChild(const core::pt32f & pos, std::shared_ptr<Control> last, findchild_flags flags) const
    {
        if (_clip_clild && !_rect.contains(pos))
            return nullptr;

        if (_invalid_layout)
        {
            Container * pthis = const_cast<Container *>(this);
            pthis->layout(_invalid_layout_flags);
            pthis->_invalid_layout = false;
            pthis->_invalid_layout_flags = nullptr;
        }

        bool found = false;

        core::pt32f scrolled_pos = pos;
        std::shared_ptr<Control> control_found = nullptr;
        for (auto & control : core::reverse(_controls))
        {
			if(!control->aviliable())
				continue;

            if (last && !found)
            {
                if (control == last)
                    found = true;
                continue;
            }

            auto hittest = control->hitTest(scrolled_pos);
            if (hittest == hittest_result::nowhere)
                continue;

            auto child = control->findChild(scrolled_pos, last, flags);
            if (child)
                return child;

            if (!flags.any(findchild_flag::disalbe_transparent) && hittest == hittest_result::transparent)
                continue;

            if (hittest == hittest_result::client || hittest == hittest_result::stable)
                return control;

            control_found = control;
        }
        return control_found;
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

    void Container::onMouseWheel(const input_state & state)
    {
        if (_layout_direction == core::align::left && _scrollbar_h)
            _scrollbar_h->onMouseWheel(state);
        else if (_layout_direction == core::align::top && _scrollbar_v)
            _scrollbar_v->onMouseWheel(state);
        Control::onMouseWheel(state);
    }

    void Container::relayout(layout_flags flags)
    {
        _invalid_layout_flags |= flags;
        if (!_invalid_layout && scene())
        {
            _invalid_layout = true;
            invoke([this]()
            {
                layout(_invalid_layout_flags);
                _invalid_layout = false;
                _invalid_layout_flags = nullptr;
				invalidate({});
            });
        }
    }

    void Container::setLayoutDirection(core::align layout)
    {
        _layout_direction = layout;
    }

    void Container::setCompactLayout(bool b)
    {
        _compact_layout = b;
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
        _animScroll({ _scroll_pos.x, to });
        relayout();
    }

    void Container::onScrollValueChangedH(float32_t from, float32_t to)
    {
        _animScroll({to, _scroll_pos.y });
        relayout();
    }

    void Container::_animScroll(core::vec2f scroll_pos)
    {
        if(!_style_transition)
        {
            _setScrollPos(scroll_pos);
            return;
        }

        if (!_scroll_anim)
        {
            auto accessor = make_accessor(&Container::_setScrollPos, &Container::_scroll_pos);
            auto interpolator = std::make_shared<core::property_interpolator_default<core::vec2f>>();
            interpolator->set_values(_scroll_pos, scroll_pos);
            _scroll_anim = std::make_shared<core::property_animation>(control_ref(), accessor, interpolator);
            _scroll_anim->setDuration(300ms);
            appendAnimation(CONTROL_ANIMATION_GROUP_CONTROL, _scroll_anim);
        }
        else
        {
            _scroll_anim->reset();
            auto & interpolator = reinterpret_cast<core::property_interpolator_default<core::vec2f> &>(*(_scroll_anim->_interpolator));
            interpolator.set_values(_scroll_pos, scroll_pos);
        }
        _scroll_anim->start();
    }

    void Container::_setScrollPos(core::vec2f scroll_pos)
    {
        _scroll_pos = scroll_pos;
        relayout();
    }

    core::si32f Container::contentSize() const
    {
        core::si32f size;
        core::rc32f box = paddingBox();
        float32_t margin = 0;
        for (auto & control : _controls)
        {
			if(!control->aviliable())
				continue;

            auto lo = control->layoutOrigin();
            if (lo != layout_origin::layout && lo != layout_origin::sticky)
                continue;

            auto m = control->realMargin();

            if (_layout_direction == core::align::left || _layout_direction == core::align::right)
            {
                margin = std::max(margin, _layout_direction == core::align::left ? m.bleft : m.bright);
                size.cx += margin;
                auto perffer_size = control->prefferSize({});
                margin = _layout_direction == core::align::left ? m.bright : m.bleft;
                size.cx = size.cx + perffer_size.cx;
                size.cy = std::max(size.cy, perffer_size.cy + m.bheight());
            }
            else if (_layout_direction == core::align::top || _layout_direction == core::align::bottom)
            {
                margin = std::max(margin, _layout_direction == core::align::top ? m.btop : m.bbottom);
                size.cy += margin;
                auto perffer_size = control->prefferSize({});
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

        scene()->setEvent(scene_event::update_mouse_pos);

        float32_t margin = 0;
        core::rc32f box = paddingBox();
        core::si32f spacing = box.size;
        core::pt32f layout_pos = _rect.pos - _scroll_pos;
        core::si32f layout_size;

        float32_t margin_size = 0;
        float32_t fixed_size = 0;
        float32_t total_per = 0;

        for (size_t cnt = 0; cnt < _controls.size(); ++cnt)
        {
            auto & control = _controls[cnt];
			if(!control->aviliable())
				continue;

            auto lo = control->layoutOrigin();
            if (lo != layout_origin::layout && lo != layout_origin::sticky)
                continue;

            auto m = control->realMargin();
            auto s = control->size();
            auto ps = control->prefferSize({});

            if (_layout_direction == core::align::left || _layout_direction == core::align::right)
            {
                margin = std::max(margin, _layout_direction == core::align::left ? m.bleft : m.bright);
                if (s.cx.avi())
                {
                    if (s.cx.per())
                        total_per += s.cx.value;
                    else
                        fixed_size += ps.cx;
                }
            }
            else if (_layout_direction == core::align::top || _layout_direction == core::align::bottom)
            {
                margin = std::max(margin, _layout_direction == core::align::top ? m.btop : m.bbottom);
                if (s.cy.avi())
                {
                    if (s.cy.per())
                        total_per += s.cy.value;
                    else
                        fixed_size += ps.cy;
                }
            }
            else {}

            margin_size += margin;

            if (_layout_direction == core::align::left || _layout_direction == core::align::right)
            {
                margin = std::max(margin, _layout_direction == core::align::left ? m.bright : m.bleft);
            }
            else if (_layout_direction == core::align::top || _layout_direction == core::align::bottom)
            {
                margin = std::max(margin, _layout_direction == core::align::top ? m.bbottom : m.btop);
            }
            else {}
        }
        margin_size += margin;

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

        spacing.cx = std::max(spacing.cx, 0.0f);
        spacing.cy = std::max(spacing.cy, 0.0f);

        margin = 0;
        float32_t compat_size = 0;
        float32_t compat_scale =0;
        std::set<std::string> setes;
        for (auto & control : _controls)
        {
			if(!control->aviliable())
				continue;

            auto margins = control->realMargin();
            auto lo = control->layoutOrigin();
            auto & si = control->size();

            if (lo == layout_origin::layout || lo == layout_origin::sticky)
            {
                switch (_layout_direction)
                {
                case core::align::left:
                {
                    //core::pt32f layout_pos = _rect.pos - _scroll_pos;
                    bool compact = _compact_layout && si.cx.per();
                    auto preffer_size = control->prefferSize({ compact ? (spacing.cx - compat_size ) / ((total_per - compat_scale) / 100.0f): spacing.cx, spacing.cy - margins.bheight() });
                    margin = std::max(margin, margins.bleft);
                    control->place({ layout_pos.x + layout_size.cx + margin, layout_pos.y + margins.btop, preffer_size.cx, box.cy - margins.bheight() }, preffer_size);
                    layout_size.cx += margin + preffer_size.cx;
                    margin = margins.bright;

                    if(compact)
                    {
                        compat_scale += si.cx.value;
                        compat_size += preffer_size.cx;
                    }
                    break;
                }
                case core::align::right:
                {
                    bool compact = _compact_layout && si.cx.per();
                    auto preffer_size = control->prefferSize({ compact ? (spacing.cx - compat_size) / ((total_per - compat_scale) / 100.0f) : spacing.cx, spacing.cy - margins.bheight() });
                    margin = std::max(margin, margins.bright);
                    control->place({ (_rect.right() - _scroll_pos.x) - (layout_size.cx + margin) - preffer_size.cx, layout_pos.y + margins.btop, preffer_size.cx, box.cy - margins.bheight() }, preffer_size);
                    layout_size.cx += margin + preffer_size.cx;
                    margin = margins.bleft;

                    if (compact)
                    {
                        compat_scale += si.cx.value;
                        compat_size += preffer_size.cx;
                    }
                    break;
                }
                case core::align::top:
                {
                    auto preffer_size = control->prefferSize({ spacing.cx - margins.bwidth(), spacing.cy });
                    margin = std::max(margin, margins.btop);
                    control->place({ layout_pos.x + margins.bleft, layout_pos.y + layout_size.cy + margin, box.cx - margins.bwidth(), preffer_size.cy }, preffer_size);
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

        if (_layout_direction == core::align::left || _layout_direction == core::align::right)
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
        case core::align::right:
            if (_scrollbar_h)
            {
                _scrollbar_h->setPageValue(contentBox().cx);
                _scrollbar_h->setValues(_scrollbar_h->value(), 0, _layouted_size.cx - contentBox().cx);
            }
            break;
        case core::align::top:
        case core::align::bottom:
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

