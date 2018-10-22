#include "stdafx.h"
#include "Control.h"
#include "Container.h"

namespace ui
{
    Control::Control() { }
    Control::~Control() { }

    void Control::propertyTableCallback(core::property_table & properties)
    {
        properties["pos"] = make_accessor(static_cast<void (Control::*)(const core::vec2<core::dimensionf> &) >(&Control::move), &Control::pos, core::parseDimension2D, nullptr);
        properties["size"] = make_accessor(static_cast<void (Control::*)(const core::vec2<core::dimensionf> &) >(&Control::resize), &Control::size, core::parseDimension2D, nullptr);

        properties["border"] = make_accessor(&Control::setBorder, &Control::border, core::parseDimension4D, nullptr);
        properties["padding"] = make_accessor(&Control::setPadding, &Control::padding, core::parseDimension4D, nullptr);
        properties["margin"] = make_accessor(&Control::setMargin, &Control::margin, core::parseDimension4D, nullptr);

        properties["border-color"] = make_accessor(&Control::setBorderColors, &Control::borderColors, core::parseColor4D, nullptr);
        properties["background-color"] = make_accessor(&Control::setBackgroundColor, &Control::backgroundColor, core::parseColor, nullptr);
    }

    void Control::propertyTable(core::property_table & properties)
    {
        propertyTableCallback(properties);
    }

    const core::property_table & Control::properties()
    {
        return core::app().properties(typeid (*this), std::bind(&Control::propertyTable, this, std::placeholders::_1));
    }

    void Control::setStyleSheet(std::shared_ptr<component::StyleSheet> styleSheet)
    {
        if (_styleSheet == styleSheet)
            return;

        _styleSheet = styleSheet;
        restyle();
    }

    std::shared_ptr<component::StyleSheet> Control::styleSheet() const
    {
        if (_styleSheet)
            return _styleSheet;

        auto s = scene();
        if (s)
            return s->styleSheet();
        return nullptr;
    }

    core::si32f Control::prefferSize(const core::vec2<float32_t> & spacing) const
    {
        if (_size.available() && _size.value.cx.avi() && _size.value.cy.avi())
        {
            core::si32f size = calc(_size, spacing);
            _adjustSize(size);
            return size;
        }

        core::si32f size = contentSize() + calc(_padding, spacing).bsize();
        if (_size.available())
        {
            if (_size.value.cx.avi())
                size.cx = calc(_size.value.cx, spacing.cx);
            else if (_size.value.cy.avi())
                size.cy = calc(_size.value.cy, spacing.cy);
            else {}
        }

        if (_anchor_borders.all(core::align::leftRight))
            size.cx = spacing.cx - calc(_anchor.value.bleft + _anchor.value.bright, spacing.cx);
        if (_anchor_borders.all(core::align::topBottom))
            size.cy = spacing.cy - calc(_anchor.value.btop + _anchor.value.bbottom, spacing.cy);
        _adjustSize(size);
        return size;
    }

    core::si32f Control::adjustSize(const core::si32f & size) const
    {
        core::si32f asize = size;
        if (_min_size.value.cx.avi())
        {
            float32_t val = calc(_min_size.value.cx);
            if (asize.cx < val)
                asize.cx = val;
        }
        if (_min_size.value.cy.avi())
        {
            float32_t val = calc(_min_size.value.cy);
            if (asize.cy < val)
                asize.cy = val;
        }

        if (_max_size.value.cx.avi())
        {
            float32_t val = calc(_max_size.value.cx);
            if (asize.cx > val)
                asize.cx = val;
        }
        if (_max_size.value.cy.avi())
        {
            float32_t val = calc(_max_size.value.cy);
            if (asize.cy > val)
                asize.cy = val;
        }
        return asize;
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

    float32_t Control::calc(const core::dimensionf & value) const
    {
        auto s = scene();
        if (!s)
            throw core::error_state;

        switch (value.unit)
        {
        case core::unit::px:
            return value.value * s->ratio();
        case core::unit::em:
			return value.value * drawing::fontmetrics(drawing::font()).height;
		case core::unit::ft:
            return value.value * drawing::fontmetrics(font()).height;
        case core::unit::pt:
            return value.value * 72.0f * s->ratio();
        case core::unit::dot:
            return value.value;
        case core::unit::per:
            return 0;
        default:
            return value.value * s->ratio();
        }
    }

    float32_t Control::calc(const core::dimensionf & value, float32_t spacing) const
    {
        auto s = scene();
        if (!s)
            throw core::error_state;

        switch (value.unit)
        {
        case core::unit::px:
            return value.value * s->ratio();
        case core::unit::em:
            return value.value * drawing::fontmetrics(drawing::font()).height;
        case core::unit::pt:
            return value.value * 72.0f * s->ratio();
        case core::unit::dot:
            return value.value;
        case core::unit::per:
            return value.value / 100.0f * spacing;
        default:
            return value.value * s->ratio();
        }
    }

    void Control::move(const core::vec2<core::dimensionf> & pos)
    {
        if (_pos != _pos)
        {
            _pos = _pos;
            setShowPos(calc(_pos));
        }
    }

    void Control::resize(const core::vec2<core::dimensionf> & size)
    {
        if(_size != size)
        {
            _size = size;
            setShowSize(calc(size));
        }
    }

    void Control::setShowPos(const core::vec2f & pos)
    {
        auto pos_old = _rect.pos;
        if (pos_old != pos)
        {
            _rect.pos = pos;
            onPosChanged(pos_old, pos);
            onRectChanged(core::rc32f(pos_old, _rect.size), core::rc32f(pos, _rect.size));
        }
    }

    void Control::setShowSize(const core::vec2f & size)
    {
        auto size_old = _rect.size;
        if (size_old != size)
        {
            _rect.size = size;
            onSizeChanged(size_old, size);
            onRectChanged(core::rc32f(_rect.pos, size_old), core::rc32f(_rect.pos, _rect.size));
        }
    }

    void Control::setShowRect(const core::rc32f & rect)
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

    core::rc32f Control::box() const
    {
        return core::rc32f(_rect.pos, _rect.size);
    }

    core::rc32f Control::borderBox() const
    {
        return core::rc32f(_rect.pos + calc(_border).bleftTop(), _rect.size - calc(_border).bsize());
    }

    core::rc32f Control::paddingBox() const
    {
        return core::rc32f(_rect.pos + calc(_padding).bleftTop(), _rect.size - calc(_padding).bsize());
    }

    core::rc32f Control::contentBox() const
    {
        return paddingBox();
    }

    core::rc32f Control::box(control_box cbox) const
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
        refresh(_rect);
    }

    void Control::refresh(const core::rc32f & rect)
    {
        _rect_invalid.unite(rect);
        if (!_delay_update)
        {
            _delay_update = true;
            invoke([this]() { update(); });
        }
    }

    void Control::rearrange()
    {
    }

    void Control::restyle()
    {
        if (!_delay_style)
        {
            _delay_style = true;
            invoke([this]() {updateStyle(); });
        }
    }

    std::array<core::pt32f, 4> Control::boderPoints(core::align edge) const
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

    std::array<core::pt32f, 2> Control::boderLine(core::align edge) const
    {
        auto bbox = box();
        auto border = calc(_border);
        switch (edge)
        {
        case core::align::left:
            return {core::pt32f{ bbox.left() + border.bleft * 0.5f, bbox.bottom() },
                core::pt32f{ bbox.left() + border.bleft * 0.5f, bbox.top() } };
        case core::align::top:
            return { core::pt32f{ bbox.left(), bbox.top() + border.btop * 0.5f },
                core::pt32f{ bbox.right(), bbox.top() + border.btop * 0.5f } };
        case core::align::right:
            return { core::pt32f{ bbox.right() - border.bright * 0.5f, bbox.top() },
                core::pt32f{ bbox.right() - border.bright * 0.5f, bbox.bottom() } };
        case core::align::bottom:
            return { core::pt32f{ bbox.right(), bbox.bottom() - border.bbottom * 0.5f },
                core::pt32f{ bbox.left(), bbox.bottom() - border.bbottom * 0.5f } };
        default:
            return {};
        }
    }

    void Control::setBackgroundImage(std::shared_ptr<drawing::Image> image)
    {
        if (image == _background_image.value)
            return;
        _background_image = image;
        refresh();
    }

    std::shared_ptr<drawing::Image> Control::backgroundImage() const
    {
        return _background_image.value;
    }

    void Control::setMargin(const core::vec4<core::dimensionf> & margin)
    {
        if (_margin != margin)
        {
            _margin = margin;
            if (auto p = parent())
                p->relayout(nullptr);
        }
    }

    void Control::onEnteringScene(std::shared_ptr<Scene> & scene)
    {
        _scene = scene;
        restyle();
    }

    void Control::onEnterScene(std::shared_ptr<Scene> & scene)
    {
    }

    void Control::onLeavingScene()
    {
        _scene.reset();
    }

    void Control::onLeaveScene() { }

    void Control::place(const core::rc32f & rect, const core::si32f & size)
    {
        assert(!std::isnan(size.cx) && !std::isnan(size.cy));
        assert(!std::isnan(rect.x) && !std::isnan(rect.y));
        assert(!std::isnan(rect.cx) && !std::isnan(rect.cy));
        assert(rect.cx < 1e6 && rect.cy < 1e6);

        core::pt32f pos;
        if (_anchor_borders.all(core::align::leftTop))
            pos = rect.leftTop();
        else if (_anchor_borders.all(core::align::rightTop))
            pos = rect.rightTop() - core::pt32f(size.cx, 0);
        else if (_anchor_borders.all(core::align::rightBottom))
            pos = rect.rightBottom() - core::pt32f(size.cx, size.cy);
        else if (_anchor_borders.all(core::align::leftBottom))
            pos = rect.leftBottom() - core::pt32f(0, size.cy);
        else
            pos = rect.leftTop();

        setShowRect({ pos, size });
    }

    void Control::updateStyle()
    {
        _delay_style = false;
        std::string style = styleName();
        if (style == _style)
            return;

        auto ss = styleSheet();
        auto s = scene();
        std::map<std::string, std::string> items = ss->generate(style);
        auto iter_transition_duration = items.find("transition-duration");
        if (_style.empty() || !_style_transition || !s || iter_transition_duration == items.end())
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
            auto duration = core::parseDuration(iter_transition_duration->second);
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

    void Control::update()
    {
        check_invoke();
		_delay_update = false;

        auto s = scene();
        auto p = parent();
        if (!s || !p)
            return;

        if (!_rect_invalid.empty() && s)
        {
            invalidate(_rect_invalid);
        }
        _rect_invalid.clear();
    }

    void Control::invalidate(const core::rc32f & rect)
    {
        if (auto p = parent())
            p->invalidate(rect);
    }

    int32_t Control::animate()
    {
        std::lock_guard lock(*this);
        int32_t num = 0;
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

    void Control::ondraw(drawing::Graphics & graphics, const core::rc32f & clip) const
    {
        uint32_t a = std::clamp< uint32_t>(uint32_t(std::round(_alpha * 0xff)), 0, 0xff);
        if (a != 0xff)
            graphics.saveLayer(box(), a);
        draw(graphics, clip);
        if (a != 0xff)
            graphics.restore();
    }

    void Control::draw(drawing::Graphics & graphics, const core::rc32f & clip) const
    {
        _drawBackground(graphics);
        _drawBorder(graphics);
    }

    void Control::onPosChanged(const core::pt32f & from, const core::pt32f & to)
    {
        posChanged(from, to);
    }

    void Control::onSizeChanged(const core::si32f & from, const core::si32f & to)
    {
        sizeChanged(from, to);
    }

    void Control::onRectChanged(const core::rc32f & from, const core::rc32f & to)
    {
        refresh(from);
        refresh(to);
        rectChanged(from, to);
    }

    void Control::onVisibleChanged(bool vis)
    {
    }

    void Control::_drawBackground(drawing::Graphics & graphics) const
    {
        if (!_background_color.available())
            return;

        graphics.drawRectangle(box(_background_box), drawing::PathStyle().fill(_background_color));
    }

    void Control::_drawBorder(drawing::Graphics & graphics) const
    {
        if (_border && _border_colors)
        {
            if (std::equal(_border.value.arr.begin() + 1, _border.value.arr.end(), _border.value.arr.begin()) &&
                std::equal(_border_colors.value.arr.begin() + 1, _border_colors.value.arr.end(), _border_colors.value.arr.begin()) &&
                std::equal(_border_styles.value.arr.begin() + 1, _border_styles.value.arr.end(), _border_styles.value.arr.begin()))
            {
                graphics.drawRectangle(box().expanded(calc(_border) * -0.5f), drawing::PathStyle().stoke(_border_colors.value.x, calc(_border.value.x), _border_styles.value[0]));
            }
            else
            {
                auto border = calc(_border);
                const core::align edges[4] = { core::align::left, core::align::top, core::align::right, core::align::bottom };
                drawing::Path path;
                for (int32_t cnt = 0; cnt < 4; ++cnt)
                {
                    if (border[cnt] > 0 && _border_colors.value[cnt].visible())
                    {
                        auto points = boderPoints(edges[cnt]);
                        auto line = boderLine(edges[cnt]);
                        auto style = drawing::PathStyle().stoke(_border_colors.value[cnt], border.arr[cnt], _border_styles.value[cnt]);

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

    void Control::_adjustSize(core::si32f & size) const
    {
        auto p = parent();
        if (p && _fixed_aspect)
        {
            auto layout_direction = p->layoutDirection();
            if (!std::isnan(_fixed_aspect.value.cx))
                size.cx = size.cy * _fixed_aspect.value.cx;
            else if (!std::isnan(_fixed_aspect.value.cy))
                size.cy = size.cx * _fixed_aspect.value.cy;
            else {}
        }

        if (_min_size.available())
        {
            if (_min_size.value.cx.avi())
            {
                float32_t val = calc(_min_size.value.cx);
                if (size.cx < val)
                    size.cx = val;
            }
            if (_min_size.value.cy.avi())
            {
                float32_t val = calc(_min_size.value.cy);
                if (size.cy < val)
                    size.cy = val;
            }
        }

        if (_max_size.available())
        {
            if (_max_size.value.cx.avi())
            {
                float32_t val = calc(_max_size.value.cx);
                if (size.cx > val)
                    size.cx = val;
            }
            if (_max_size.value.cy.avi())
            {
                float32_t val = calc(_max_size.value.cy);
                if (size.cy > val)
                    size.cy = val;
            }
        }
    }

    void Control::_adjustSize(core::si32f & size, const core::vec2<float32_t> & spacing) const
    {
        auto p = parent();
        if (p && _fixed_aspect)
        {
            auto layout_direction = p->layoutDirection();
            if (!std::isnan(_fixed_aspect.value.cx))
                size.cx = size.cy * _fixed_aspect.value.cx;
            else if (!std::isnan(_fixed_aspect.value.cy))
                size.cy = size.cx * _fixed_aspect.value.cy;
            else {}
        }

        if (_min_size.available())
        {
            if (_min_size.value.cx.avi())
            {
                float32_t val = calc(_min_size.value.cx, spacing.cx);
                if (size.cx < val)
                    size.cx = val;
            }
            if (_min_size.value.cy.avi())
            {
                float32_t val = calc(_min_size.value.cy, spacing.cy);
                if (size.cy < val)
                    size.cy = val;
            }
        }

        if (_max_size.available())
        {
            if (_max_size.value.cx.avi())
            {
                float32_t val = calc(_max_size.value.cx, spacing.cx);
                if (size.cx > val)
                    size.cx = val;
            }
            if (_max_size.value.cy.avi())
            {
                float32_t val = calc(_max_size.value.cy, spacing.cy);
                if (size.cy > val)
                    size.cy = val;
            }
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
        animation->started += std::weak_bind(&Control::_onAnimationStarted, control_ref());
        _animations[group].push_back(animation);
        auto s = scene();
        if (s)
            s->animate();
    }

    void Control::_onAnimationStarted()
    {
        auto s = scene();
        if (s)
            s->animate();
    }

    void Control::onMouseEnter(const input_state & state)
    {
        if (auto s = scene())
        {
            s->cursorContext()->setCursor(_cursor);
        }
        _mousein = true;
        mouseEnter(state);
    }

    void Control::onMouseLeave(const input_state & state)
    {
        if (auto s = scene())
        {
            s->cursorContext()->resetCursor();
        }
        _pressed = false;
        _mousein = false;
        mouseLeave(state);
    }

}
