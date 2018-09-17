#include "stdafx.h"
#include "Control.h"

namespace ui
{
    const std::string CONTROL_ANIMATION_GROUP_STYLE = "_style";

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

    core::si32f Control::prefferSize(calc_flags flags) const
    {
        // 如果设置了固定大小，直接返回即可
        if (_size.available() && _size.value.cx.avi() && _size.value.cy.avi())
            return calc(_size.value, flags);

        core::si32f size = contentSize() + calc(_padding).bsize();
        if (_size.available())
        {
            if (_size.value.cx.avi())
                size.cx = calc_x(_size.value.cx, flags);
            else if (_size.value.cy.avi())
                size.cy = calc_y(_size.value.cy, flags);
            else {}
        }

        auto p = parent();
        if (_anchor_borders.all(core::align::leftRight))
            size.cx = p->width() - (calc_x(_anchor.value.bleft) + calc_x(_anchor.value.bright));
        if (_anchor_borders.all(core::align::topBottom))
            size.cy = p->width() - (calc_y(_anchor.value.btop) + calc_y(_anchor.value.bbottom));
        _adjustSizeMinMax(size);
        return size;
    }

    const core::color32 & Control::color() const
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

    void Control::enteringScene(std::shared_ptr<Scene> & scene)
    {
        _scene = scene;
        restyle();
    }

    void Control::enterScene(std::shared_ptr<Scene> & scene)
    {
    }

    void Control::leavingScene()
    {
        _scene.reset();
    }

    void Control::leaveScene() { }

    void Control::place(const core::rc32f & rect, const core::si32f & size)
    {
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
        if (!s)
            return;

        if (!_rect_invalid.empty() && s)
            s->invalid(_rect_invalid);
        _rect_invalid.clear();
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

    void Control::render(drawing::Graphics & graphics, const drawing::Region & region) const
    {
        _renderBackground(graphics);
        _renderBorder(graphics);
    }

    void Control::onPosChanged(const core::pt32f & from, const core::pt32f & to)
    {
        refresh();
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

    void Control::_renderBackground(drawing::Graphics & graphics) const
    {
        if (!_background_color.available())
            return;

        graphics.drawRectangle(box(_background_box), drawing::PathStyle().fill(_background_color));
    }

    void Control::_renderBorder(drawing::Graphics & graphics) const
    {
        if (_border && _border_colors)
        {
            if (std::equal(_border.value.arr.begin() + 1, _border.value.arr.end(), _border.value.arr.begin()) &&
                std::equal(_border_colors.value.arr.begin() + 1, _border_colors.value.arr.end(), _border_colors.value.arr.begin()) &&
                std::equal(_border_styles.value.arr.begin() + 1, _border_styles.value.arr.end(), _border_styles.value.arr.begin()))
            {
                graphics.drawRectangle(box().expanded(calc(_border) * -0.5f), drawing::PathStyle().stoke(_border_colors.value.x, _border_styles.value[0]).width(calc_x(_border.value.x)));
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
                        auto style = drawing::PathStyle().stoke(_border_colors.value[cnt], _border_styles.value[cnt]).width(border.arr[cnt]);

                        path.clear();
                        path.fromPoints(std::begin(points), std::end(points), true);

                        graphics.save();
                        graphics.setClipPath(path);
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

    void Control::_adjustSizeMinMax(core::si32f & size) const
    {
        if (_minSize.available())
        {
            if (_minSize.value.cx.avi())
            {
                float32_t val = calc_x(_minSize.value.cx);
                if (size.cx < val)
                    size.cx = val;
            }
            if (_minSize.value.cy.avi())
            {
                float32_t val = calc_y(_minSize.value.cy);
                if (size.cy < val)
                    size.cy = val;
            }
        }

        if (_maxSize.available())
        {
            if (_maxSize.value.cx.avi())
            {
                float32_t val = calc_x(_maxSize.value.cx);
                if (size.cx > val)
                    size.cx = val;
            }
            if (_maxSize.value.cy.avi())
            {
                float32_t val = calc_y(_maxSize.value.cy);
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
}
