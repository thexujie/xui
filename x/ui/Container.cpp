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
            _controls.insert(control);
            control->onEnterScene(s);
        }
        else
            _controls.insert(control);
        relayout();
    }

    std::shared_ptr<controls::Spacer> Container::addSpacer(core::dimenf size)
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

    void Container::invalidate(const core::rectf & rect)
    {
        if(auto p = parent())
			p->invalidate(rect);
    }

	bool Container::validCompleted() const
    {
		if(_delay_repaint || _invalid_layout)
			return false;

		for(auto & control : _controls)
		{
			if(!control->validCompleted())
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

    void Container::prepaint(drawing::Graphics & graphics, const core::rectf & clip) const
    {
        uint32_t a = std::clamp< uint32_t>(uint32_t(std::round(_alpha * 0xff)), 0, 0xff);
        std::lock_guard lock(*this);
        if (a != 0xff)
            graphics.saveLayer(box().expanded(1.0f), a);
        else
            graphics.save();
        _drawBackground(graphics);
        paint(graphics, clip);
        if (_clip_clild)
            graphics.setClipRect(_rect);
        auto clip2 = _clip_clild ? clip.intersected(_rect) : clip;
        for (auto & control : _controls)
        {
			if(!control->aviliable())
				continue;

            auto rect = control->realRect();
            if(clip2.intersect_with(rect))
                control->prepaint(graphics, clip2);
        }
        _drawBorder(graphics);
        graphics.restore();
      }

    std::shared_ptr<Control> Container::findChild(const core::pointf & pos, std::shared_ptr<Control> last, findchild_flags flags) const
    {
        if (_clip_clild && !_rect.contains(pos))
            return nullptr;

        const_cast<Container *>(this)->_confirmLayout();

        bool found = false;

        core::pointf scrolled_pos = pos;
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

    void Container::onLayoutedSizeChaged(const core::sizef & from, const core::sizef & to)
    {
        layoutedSizeChaged(from, to);
    }

    void Container::setLayoutedSize(const core::sizef & layouted_size)
    {
        if (layouted_size != _layouted_size)
        {
            _layouted_size = layouted_size;

            if (_scrollbar_h)
            {
                _scrollbar_h->setPageValue(contentBox().cx);
                _scrollbar_h->setValues(_scrollbar_h->value(), 0, _layouted_size.cx - contentBox().cx);
            }

            if (_scrollbar_v)
            {
                _scrollbar_v->setPageValue(contentBox().cy);
                _scrollbar_v->setValues(_scrollbar_v->value(), 0, _layouted_size.cy - contentBox().cy);
            }
        }
    }


    void Container::onPosChanged(const core::pointf & from, const core::pointf & to)
    {
        relayout();
        Control::onPosChanged(from, to);
    }

    void Container::onSizeChanged(const core::sizef & from, const core::sizef & to)
    {
        if (_scrollbar_h)
        {
            _scrollbar_h->setPageValue(contentBox().cx);
            _scrollbar_h->setValues(_scrollbar_h->value(), 0, _layouted_size.cx - contentBox().cx);
        }

        if (_scrollbar_v)
        {
            _scrollbar_v->setPageValue(contentBox().cy);
            _scrollbar_v->setValues(_scrollbar_v->value(), 0, _layouted_size.cy - contentBox().cy);
        }

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
        if (_scrollbar_v)
            _scrollbar_v->onMouseWheel(state);
        else if (_scrollbar_h)
            _scrollbar_h->onMouseWheel(state);
        else {}
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
                    _scrollbar_v->setPlaceAnchor(core::align::right | core::align::topBottom);
                    _scrollbar_v->setLayoutOrigin(layout_origin::parent);
                    _scrollbar_v->setZValue(ZVALUE_SCROLLBAR);
                    _scrollbar_v->valueChagned += std::weak_bind(&Container::onScrollBarValueChangedV, share_ref<Container>(), std::placeholders::_1, std::placeholders::_2);
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
                    _scrollbar_h->setPlaceAnchor(core::align::bottom | core::align::leftRight);
                    _scrollbar_h->setLayoutOrigin(layout_origin::parent);
                    _scrollbar_h->setZValue(ZVALUE_SCROLLBAR);
                    _scrollbar_h->setDirection(core::align::left);
                    _scrollbar_h->valueChagned += std::weak_bind(&Container::onScrollBarValueChangedH, share_ref<Container>(), std::placeholders::_1, std::placeholders::_2);
                }
                addControl(_scrollbar_h);
            }
        }
    }

    void Container::onScrollBarValueChangedV(float32_t from, float32_t to)
    {
        _animScroll({ _scroll_pos.x, to });
        relayout();
    }

    void Container::onScrollBarValueChangedH(float32_t from, float32_t to)
    {
        _animScroll({to, _scroll_pos.y });
        relayout();
    }

    void Container::onScrollPosChanged(const core::vec2f & from, const core::vec2f & to)
    {
    }

	void Container::_confirmLayout() const
    {
		if(_invalid_layout)
		{
			Container * pthis = const_cast<Container *>(this);
			pthis->layout(_invalid_layout_flags);
			pthis->_invalid_layout = false;
			pthis->_invalid_layout_flags = nullptr;
		}
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
            _scroll_anim->setCurve(core::easingcurve_out_cubic);
            //_scroll_anim->setCurve([](float vv) { return 1 - std::pow(1.0f - vv, 8); });
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
        auto old = _scroll_pos;
        _scroll_pos = scroll_pos;
        if (_scroll_controls)
            relayout();
        else
            refresh();
        onScrollPosChanged(old, _scroll_pos);
    }

    core::sizef Container::contentSize() const
    {
        core::sizef size;
        core::rectf box = paddingBox();
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

        float32_t margin = 0;
        core::rectf box = contentBox();
        core::sizef spacing = box.size;
        core::pointf layout_pos = box.pos - _scroll_pos;
        core::sizef layout_size;

        float32_t margin_size = 0;
        float32_t fixed_size = 0;
        float32_t total_per = 0;

        for (auto & control : _controls)
        {
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
                else
                    fixed_size += ps.cx;
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
                else
                    fixed_size += ps.cy;
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
                    //core::pointf layout_pos = _rect.pos - _scroll_pos;
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

        if(_scroll_controls)
            setLayoutedSize(layout_size);

        scene()->setEvent(scene_event::update_mouse_pos);
    }
}

