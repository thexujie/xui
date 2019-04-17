#include "stdafx.h"
#include "Control.h"
#include "Container.h"
#include <optional>
#include "Desktop.h"

namespace ui
{
    Control::Control() { }
    Control::~Control() { }

    void Control::propertyTableCallback(core::property_table & properties)
    {
        properties["pos"] = make_accessor(static_cast<void (Control::*)(const core::pointf &) >(&Control::move), &Control::pos);
        properties["size"] = make_accessor(static_cast<void (Control::*)(const core::vec2<core::dimenf> &) >(&Control::resize), &Control::size);

        properties["border"] = make_accessor(&Control::setBorder, &Control::border);
        properties["padding"] = make_accessor(&Control::setPadding, &Control::padding);
        properties["margin"] = make_accessor(&Control::setMargin, &Control::margin);

        properties["border-color"] = make_accessor(&Control::setBorderColors, &Control::borderColors);
        properties["background-color"] = make_accessor(&Control::setBackgroundColor, &Control::backgroundColor);
    }

    void Control::propertyTable(core::property_table & properties)
    {
        propertyTableCallback(properties);
    }

    const core::property_table & Control::properties()
    {
        return core::app().properties(typeid (*this), std::bind(&Control::propertyTable, this, std::placeholders::_1));
    }

    void Control::setContentSize(const core::sizef & size)
    {
        if (_content_size != size)
        {
            _content_size = size;
            if(!_size.cx.avi() || !_size.cy.avi())
                replace();
        }
    }

    core::sizef Control::prefferSize() const
    {
        valid();
        if (_size.cx.avi() && _size.cy.avi())
        {
            core::sizef size = calc(_size);
            _adjustSize(size);
            return size;
        }

        core::sizef size = contentSize() + edges().bsize();
        if (_size.cx.avi() && !_size.cx.per())
            size.cx = calc(_size.cx);
        else if (_size.cy.avi() && !_size.cy.per())
            size.cy = calc(_size.cy);
        else {}

        _adjustSize(size);
        return size;
    }

    core::sizef Control::adjustSize(const core::sizef & size) const
    {
        core::sizef asize = size;
        if (_min_size.cx.avi())
        {
            float32_t val = calc(_min_size.cx);
            if (asize.cx < val)
                asize.cx = val;
        }
        if (_min_size.cy.avi())
        {
            float32_t val = calc(_min_size.cy);
            if (asize.cy < val)
                asize.cy = val;
        }

        if (_max_size.cx.avi())
        {
            float32_t val = calc(_max_size.cx);
            if (asize.cx > val)
                asize.cx = val;
        }
        if (_max_size.cy.avi())
        {
            float32_t val = calc(_max_size.cy);
            if (asize.cy > val)
                asize.cy = val;
        }
        return asize;
    }

    std::shared_ptr<Form> Control::form() const
    {
        if (auto p = parent())
            return p->form();
        else
            return nullptr;
    }

    std::shared_ptr<component::StyleSheet> Control::styleSheet() const
    {
        if (auto p = parent())
            return p->styleSheet();
        else
            return nullptr;
    }

    std::shared_ptr<ui::IImeContext> Control::imeContext() const
    {
        if (auto p = parent())
            return p->imeContext();
        else
            return nullptr;
    }

    std::shared_ptr<ICursorContext> Control::cursorContext() const
    {
        if (auto p = parent())
            return p->cursorContext();
        else
            return nullptr;
    }

    float_t Control::ratio() const
    {
        if (auto p = parent())
            return p->ratio();
        else
            return Desktop::instance().scale();
    }

    const core::color & Control::color() const
    {
        if (_color.available())
            return _color;

        auto p = parent();
        if (p)
            return p->color();
        else
            return _color;
    }

    void Control::setFont(const drawing::font & font)
    {
        _font = font;
        refresh();
    }

    const drawing::font & Control::font() const
    {
        if (_font.available())
            return _font;

        auto p = parent();
        if (p)
            return p->font();
        else
            return _font;
    }

    float32_t Control::calc(const core::dimenf & value) const
    {
        switch (value.unit)
        {
        case core::unit::px:
            return value.value * ratio();
        case core::unit::em:
			return value.value * drawing::default_fontmetrics().height;
		case core::unit::ft:
            return value.value * drawing::fontmetrics(font()).height;
        case core::unit::pt:
            return value.value * 72.0f * ratio();
        case core::unit::dot:
            return value.value;
        case core::unit::per:
            return 0;
        default:
            return value.value * ratio();
        }
    }

    float32_t Control::calc(const core::dimenf & value, float32_t spacing) const
    {
        switch (value.unit)
        {
        case core::unit::px:
            return value.value * ratio();
        case core::unit::em:
            return value.value * drawing::fontmetrics(drawing::font()).height;
        case core::unit::pt:
            return value.value * 72.0f * ratio();
        case core::unit::dot:
            return value.value;
        case core::unit::per:
            return value.value / 100.0f * spacing;
        default:
            return value.value * ratio();
        }
    }

    void Control::move(const core::pointf & pos)
    {
        if (_rect.pos != pos)
        {
            auto pos_old = _rect.pos;
            _rect.pos = pos;
            onPosChanged(pos_old, pos);
            onRectChanged(core::rectf(pos_old, _rect.size), core::rectf(pos, _rect.size));
        }
    }

    void Control::resize(const core::vec2<core::dimenf> & size)
    {
        if(_size != size)
        {
            _size = size;
            setShowSize(calc(size));
        }
    }

    void Control::setShowSize(const core::vec2f & size)
    {
        auto size_old = _rect.size;
        if (size_old != size)
        {
            _rect.size = size;
            onSizeChanged(size_old, size);
            onRectChanged(core::rectf(_rect.pos, size_old), core::rectf(_rect.pos, _rect.size));
        }
    }

    void Control::setShowRect(const core::rectf & rect)
    {
        auto rect_old = _rect;
        bool rectchanged = false;
        if (rect_old.pos != rect.pos)
        {
            _rect.pos = rect.pos;
            onPosChanged(rect_old.pos, rect.pos);
            rectchanged = true;
        }
        if (rect_old.size != rect.size)
        {
            _rect.size = rect.size;
            onSizeChanged(rect_old.size, rect.size);
            rectchanged = true;
        }
        if(rectchanged)
            onRectChanged(rect_old, rect);
    }

    core::rectf Control::box() const
    {
        return core::rectf(_rect.pos, _rect.size);
    }

    core::rectf Control::borderBox() const
    {
        return core::rectf(_rect.pos + calc(_border).bleftTop(), _rect.size - calc(_border).bsize());
    }

    core::rectf Control::paddingBox() const
    {
        return core::rectf(_rect.pos + calc(_padding).bleftTop(), _rect.size - calc(_padding).bsize());
    }

    core::rectf Control::contentBox() const
    {
        core::vec4f e = edges();
        auto box = core::rectf(_rect.pos + e.bleftTop(), _rect.size - e.bsize());
        return box.empty() ? core::vec4f() : box;
    }

    core::rectf Control::box(control_box cbox) const
    {
        switch (cbox)
        {
        case control_box::layout_box: return box();
        case control_box::border_box: return borderBox();
        case control_box::padding_box: return paddingBox();
        case control_box::content_box: return contentBox();
        default: return borderBox();
        }
    }

    void Control::refresh()
    {
        if (!_delay_refresh)
        {
            _delay_refresh = true;
            invoke([this]()
            {
                _delay_refresh = false;
                update();
            });
        }
    }

    void Control::repaint()
    {
        check_invoke();
        if (!_delay_repaint)
        {
            _delay_repaint = true;
            _rect_repaint = _rect;
            invoke([this]()
            {
                invalidate();
            });
        }
        else
            _rect_repaint.unite(_rect);
    }

    void Control::repaint(const core::rectf & rect)
    {
		if (_aviliable && _shown)
			invalidate(rect);
    }

    void Control::replace()
    {
        if (auto p = parent())
            p->relayout();
    }

    void Control::restyle()
    {
        if (!_delay_restyle)
        {
            _delay_restyle = true;
            invoke([this]() {updateStyle(); });
        }
    }

    std::array<core::pointf, 4> Control::boderPoints(core::align edge) const
    {
        auto bbox = box();
        auto border = calc(_border);
        switch (edge)
        {
        case core::align::left:
            return { bbox.leftBottom(), bbox.leftTop(),
            { bbox.left() + border.bleft * 0.5f, bbox.top() + border.btop * 0.5f },
            { bbox.left() + border.bleft * 0.5f, bbox.bottom() - border.bbottom * 0.5f } };
        case core::align::top:
            return { bbox.leftTop(), bbox.rightTop(),
            { bbox.right() - border.bright * 0.5f, bbox.top() + border.btop * 0.5f },
            { bbox.left() + border.bleft * 0.5f, bbox.top() + border.btop * 0.5f } };
        case core::align::right:
            return { bbox.rightTop(), bbox.rightBottom(),
            { bbox.right() - border.bright * 0.5f, bbox.bottom() - border.bbottom * 0.5f },
            { bbox.right() - border.bright * 0.5f, bbox.top() + border.btop * 0.5f } };
        case core::align::bottom:
            return { bbox.rightBottom(), bbox.leftBottom(),
            { bbox.left() + border.bleft * 0.5f, bbox.bottom() - border.bbottom * 0.5f },
            { bbox.right() - border.bright * 0.5f, bbox.bottom() - border.bbottom * 0.5f } };
        default:
            return {};
        }
    }

    std::array<core::pointf, 2> Control::boderLine(core::align edge) const
    {
        auto bbox = box();
        auto border = calc(_border);
        switch (edge)
        {
        case core::align::left:
            return {core::pointf{ bbox.left() + border.bleft * 0.5f, bbox.bottom() },
                core::pointf{ bbox.left() + border.bleft * 0.5f, bbox.top() } };
        case core::align::top:
            return { core::pointf{ bbox.left(), bbox.top() + border.btop * 0.5f },
                core::pointf{ bbox.right(), bbox.top() + border.btop * 0.5f } };
        case core::align::right:
            return { core::pointf{ bbox.right() - border.bright * 0.5f, bbox.top() },
                core::pointf{ bbox.right() - border.bright * 0.5f, bbox.bottom() } };
        case core::align::bottom:
            return { core::pointf{ bbox.right(), bbox.bottom() - border.bbottom * 0.5f },
                core::pointf{ bbox.left(), bbox.bottom() - border.bbottom * 0.5f } };
        default:
            return {};
        }
    }

    void Control::setBackgroundImage(std::shared_ptr<drawing::Image> image)
    {
        if (image == _background_image.value)
            return;
        _background_image = image;
        repaint();
    }

    std::shared_ptr<drawing::Image> Control::backgroundImage() const
    {
        return _background_image.value;
    }

    void Control::setMargin(const core::vec4<core::dimenf> & margin)
    {
        if (_margin != margin)
        {
            _margin = margin;
            replace();
        }
    }

    core::vec4f Control::edges() const
    {
        auto padding = calc(_padding);
        auto border = calc(_border);
        return { std::max(padding.bleft, border.bleft) ,
            std::max(padding.btop, border.btop) ,
            std::max(padding.bright, border.bright) ,
            std::max(padding.bbottom, border.bbottom) };
    }

    void Control::onEntering(std::shared_ptr<Form> & form)
    {
        restyle();
    }

    void Control::onEnter(std::shared_ptr<Form> & form)
    {
        repaint();
    }

    void Control::onLeaving()
    {
    }

    void Control::onLeave() { }

    void Control::place(const core::rectf & rect, const core::sizef & size)
    {
        assert(!std::isnan(size.cx) && !std::isnan(size.cy));
        assert(!std::isnan(rect.x) && !std::isnan(rect.y));
        assert(!std::isnan(rect.cx) && !std::isnan(rect.cy));
        assert(rect.cx < 1e6 && rect.cy < 1e6);

        core::pointf pos = rect.leftTop();
        if(_place_alignment)
        {
            if (_place_alignment.value.any(core::align::right))
                pos.x = rect.right() - size.cx;
            else if (_place_alignment.value.any(core::align::centerX))
                pos.x = rect.centerX() - size.cx / 2;
            else
                pos.x = rect.x;

            if (_place_alignment.value.any(core::align::bottom))
                pos.y = rect.bottom() - size.cy;
            else if (_place_alignment.value.any(core::align::centerY))
                pos.y = rect.centerY() - size.cy / 2;
            else
                pos.y = rect.y;
        }
        else
        {
            if (_anchor.bright.avi())
                pos.x = rect.right() - size.cx;
            if (_anchor.bbottom.avi())
                pos.y = rect.bottom() - size.cy;
        }

        setShowRect({ pos, size });
    }

    void Control::setEvent(scene_event evt)
    {
        if (auto p = parent())
            p->setEvent(evt);
    }

    void Control::valid() const
    {
        if(_delay_restyle)
        {
            const_cast<Control *>(this)->_delay_restyle = false;
            const_cast<Control *>(this)->updateStyle();
        }
        //if (_delay_relayout)
        //{
        //    const_cast<Control *>(this)->_delay_relayout = false;
        //    const_cast<Control *>(this)->layout();
        //}
        if(_delay_refresh)
        {
            const_cast<Control *>(this)->_delay_refresh = false;
            const_cast<Control *>(this)->update();
        }
    }

    void Control::updateStyle()
    {
        _delay_restyle = false;
        std::string style = styleName();
        if (style == _style)
            return;

        auto ss = styleSheet();
        if (!ss)
            return;

        std::map<std::string, std::string> items = ss->generate(style);
        auto iter_transition_duration = items.find("transition-duration");
        if (_style.empty() || !_style_transition || iter_transition_duration == items.end())
        {
            clearAnimations(CONTROL_ANIMATION_GROUP_STYLE);
            auto & props = properties();
            for (const auto & item : items)
            {
                auto iter = props.find(item.first);
                if (iter != props.end())
                    iter->second->set(item.second, *this);
            }
        }
        else
        {
            auto duration = core::property_parser<std::chrono::nanoseconds>(iter_transition_duration->second);
            items.erase(iter_transition_duration);

            clearAnimations(CONTROL_ANIMATION_GROUP_STYLE);
            auto & props = properties();
            for(auto & item : items)
            {
                auto iter = props.find(item.first);
                if (iter == props.end())
                    continue;

                auto accessor = iter->second;
                auto interpolator = accessor->create_interpolator();
                if(interpolator)
                {
                    accessor->get(*this, interpolator->start());
                    accessor->serialize(item.second, interpolator->end());
                    if (interpolator->start().equal(interpolator->end()))
                        continue;

                    auto anim = std::make_shared<core::property_animation>(shared_from_this(), accessor, interpolator);
                    anim->setDuration(duration);
                    anim->start();
                    appendAnimation(CONTROL_ANIMATION_GROUP_STYLE, anim);
                }
                else
                {
                    iter->second->set(item.second, *this);
                }
            }
        }
        _style = style;
    }

    void Control::invalidate()
    {
        if(_delay_repaint && !_delay_relayout)
        {
            _delay_repaint = false;
            invalidate(_rect_repaint);
            _rect_repaint.clear();
        }
    }

    void Control::invalidate(const core::rectf & rect)
    {
        if (auto p = parent())
            p->invalidate(rect);
    }

    void Control::animate()
    {
        if (auto p = parent())
            p->animate();
    }

    size_t Control::onAnimate()
    {
        size_t num = 0;
        for (auto & animations : _animations)
        {
            for (auto & anim : animations.second)
            {
                if (!anim->pausing() && anim->update())
                    ++num;
            }
        }
        return num;
    }

    void Control::onPaint(drawing::Graphics & graphics, const core::rectf & clip) const
    {
		if(!_visible)
			return;

        uint32_t a = std::clamp< uint32_t>(uint32_t(std::round(_alpha * 0xff)), 0, 0xff);
        if (a != 0xff)
            graphics.saveLayer(box(), a);
		{
            _drawBackground(graphics);
            paint(graphics, clip);
            _drawBorder(graphics);
		}
        if (a != 0xff)
            graphics.restore();
    }

    void Control::paint(drawing::Graphics & graphics, const core::rectf & clip) const
    {
    }

    void Control::onPosChanged(const core::pointf & from, const core::pointf & to)
    {
        posChanged(from, to);
    }

    void Control::onSizeChanged(const core::sizef & from, const core::sizef & to)
    {
        sizeChanged(from, to);
    }

    void Control::onRectChanged(const core::rectf & from, const core::rectf & to)
    {
        repaint(from);
        repaint(to);
        rectChanged(from, to);
    }

    void Control::onVisibleChanged(bool vis)
    {
		repaint();
    }

	void Control::onAviliableChanged(bool avi)
    {
		if(auto p = parent())
		{
			p->relayout();
			p->repaint();
		}
    }

    void Control::_drawBackground(drawing::Graphics & graphics) const
    {
        if (!_background_color.available())
            return;

        graphics.drawRectangle(box(_background_box), drawing::PathStyle().fill(_background_color));
    }

    void Control::_drawBorder(drawing::Graphics & graphics) const
    {
        if (_border.x.avi() && _border_colors.x.visible())
        {
            if (std::equal(_border.arr.begin() + 1, _border.arr.end(), _border.arr.begin()) &&
                std::equal(_border_colors.arr.begin() + 1, _border_colors.arr.end(), _border_colors.arr.begin()) &&
                std::equal(_border_styles.arr.begin() + 1, _border_styles.arr.end(), _border_styles.arr.begin()))
            {
                graphics.drawRectangle(box().expanded(calc(_border) * -0.5f), drawing::PathStyle().stoke(_border_colors.x, calc(_border.x), _border_styles[0]));
            }
            else
            {
                auto border = calc(_border);
                const core::align edges[4] = { core::align::left, core::align::top, core::align::right, core::align::bottom };
                drawing::Path path;
                for (int32_t cnt = 0; cnt < 4; ++cnt)
                {
                    if (border[cnt] > 0 && _border_colors[cnt].visible())
                    {
                        auto points = boderPoints(edges[cnt]);
                        auto line = boderLine(edges[cnt]);
                        auto style = drawing::PathStyle().stoke(_border_colors[cnt], border.arr[cnt], _border_styles[cnt]);

                        path.clear();
                        path.fromPoints(std::begin(points), std::end(points), true);

                        graphics.save();
                        //graphics.setClipPath(path);
                        graphics.drawLine(line[0], line[1], style);
                        graphics.restore();
                    }
                    else
                    {
                    }
                }
            }
        }
    }

    void Control::_adjustSize(core::sizef & size) const
    {
        auto p = parent();
        if (p && !std::isnan(_fixed_aspect.cx))
        {
            if (!std::isnan(_fixed_aspect.cx))
                size.cx = size.cy * _fixed_aspect.cx;
            else if (!std::isnan(_fixed_aspect.cy))
                size.cy = size.cx * _fixed_aspect.cy;
            else {}
        }

        if (_min_size.cx.avi())
        {
            float32_t val = calc(_min_size.cx);
            if (size.cx < val)
                size.cx = val;
        }
        if (_min_size.cy.avi())
        {
            float32_t val = calc(_min_size.cy);
            if (size.cy < val)
                size.cy = val;
        }

        if (_max_size.cx.avi())
        {
            float32_t val = calc(_max_size.cx);
            if (size.cx > val)
                size.cx = val;
        }
        if (_max_size.cy.avi())
        {
            float32_t val = calc(_max_size.cy);
            if (size.cy > val)
                size.cy = val;
        }
    }

    void Control::clearAnimations(std::string group)
    {
        auto iter = _animations.find(group);
        if (iter != _animations.end())
            _animations.erase(iter);
    }

    void Control::appendAnimation(std::string group, std::shared_ptr<core::animation> animation)
    {
        animation->started += std::bind(&Control::animate, this);
        _animations[group].push_back(animation);
        animate();
    }

	void Control::notifyHovered(const input_state & state, bool hovered)
    {
		if (auto cc = cursorContext())
			hovered ? cc->setCursor(_cursor) : cc->resetCursor();
		setHovered(hovered);
		hovered ? onHoverIn(state) : onHoverOut(state);
	}

	void Control::notifySelected(const input_state & state, bool selected)
    {
		setSelected(selected);
		selected ? onSelectIn(state) : onSelectOut(state);
    }

	void Control::notifyActived(const input_state & state, bool actived)
    {
		setActived(actived);
		actived ? onActiveIn(state) : onActiveOut(state);
    }

	void Control::notifyFocused(const input_state & state, bool focused)
    {
		setFocused(focused);
		focused ? onFocusIn(state) : onFocusOut(state);
    }

	void Control::notifyShown(bool shown)
    {
        if (shown == _shown)
            return;

        repaint();
		setShown(shown);
        repaint();
		shown ? onShow() : onHide();
    }
}
