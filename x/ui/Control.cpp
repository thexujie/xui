#include "stdafx.h"
#include "Control.h"
#include "renderables/Image.h"
#include "renderables/Rectangle.h"
#include "renderables/Path.h"
#include "renderables/Line.h"

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
        updateStyle();
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

    core::si32f Control::prefferSize(calc_flag flags) const
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

        _adjustSizeMinMax(size);
        return size;
    }

    std::shared_ptr<component::Animation> Control::animation() const
    {
        if (!_animation)
            const_cast<std::shared_ptr<component::Animation> &>(_animation) = std::make_shared<component::Animation>();
        return _animation;
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

    const graphics::font & Control::font() const
    {
        if (_font.available())
            return _font;

        auto p = parent();
        if (p)
            return p->font();
        else
            return _font;
    }

    float32_t Control::calc_x(const core::dimensionf & value, calc_flag flags) const
    {
        auto s = scene();
        auto p = parent();
        if (!s)
            throw core::error_state;

        switch (value.unit)
        {
        case core::unit::px:
            return value.value * s->ratio();
        case core::unit::em:
            return value.value * font().size * s->ratio();
        case core::unit::pt:
            return value.value * 72.0f * s->ratio();
        case core::unit::dot:
            return value.value;
        case core::unit::per:
            if (core::testbit(flags, calc_flag::donot_calc_percent_x))
                return 0;
            if (!p)
                throw core::error_state;
            return value.value / 100.0f * p->width();
        default:
            return value.value * s->ratio();
        }
    }

    float32_t Control::calc_y(const core::dimensionf & value, calc_flag flags) const
    {
        auto s = scene();
        auto p = parent();
        if (!s)
            throw core::error_state;

        switch (value.unit)
        {
        case core::unit::px:
            return value.value * s->ratio();
        case core::unit::em:
            return value.value * font().size * s->ratio();
        case core::unit::pt:
            return value.value * 72.0f * s->ratio();
        case core::unit::dot:
            return value.value;
        case core::unit::per:
            if (core::testbit(flags, calc_flag::donot_calc_percent_y))
                return 0;
            if (!p)
                throw core::error_state;
            return value.value / 100.0f * p->height();
        default:
            return value.value * s->ratio();
        }
    }

    core::vec2f Control::calc(const core::vec2<core::dimensionf> & value, calc_flag flags) const
    {
        return { calc_x(value.x, flags), calc_y(value.y, flags) };
    }

    core::vec4f Control::calc(const core::vec4<core::dimensionf> & value, calc_flag flags) const
    {
        return { calc(value.xy, flags), calc(value.zw, flags) };
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

    void Control::invalid()
    {
        if(!_invalid)
        {
            _invalid = true;
            invoke([this]() {update(); });
        }
    }

    void Control::relayout()
    {
        if (!_invalid_layout)
        {
            _invalid_layout = true;
            invoke([this]() {layout(); });
        }
    }

    void Control::rearrange()
    {
        auto p = parent();
        if (p)
            p->relayout();
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

    void Control::setBackgroundColor(core::color32 color)
    {
        if (color == _background_color.value)
            return;

        _background_color = color;
        invalid();
    }

    core::color32 Control::backgroundColor() const
    {
        return _background_color;
    }

    void Control::setBackgroundImage(std::shared_ptr<graphics::Image> image)
    {
        if (image == _background_image.value)
            return;
        _background_image = image;
        invalid();
    }

    std::shared_ptr<graphics::Image> Control::backgroundImage() const
    {
        return _background_image.value;
    }

    void Control::enteringScene(std::shared_ptr<component::Scene> & scene)
    {
        _scene = scene;
        auto c = share_ref<Control>();
        scene->insert(c);
        updateStyle();
    }

    void Control::enterScene(std::shared_ptr<component::Scene> & scene)
    {
    }

    void Control::leavingScene(std::shared_ptr<component::Scene> & scene)
    {
        _scene.reset();
        auto c = share_ref<Control>();
        scene->remove(c);
    }

    void Control::leaveScene(std::shared_ptr<component::Scene> & scene) { }

    void Control::layout()
    {
        _invalid_layout = false;
    }

    void Control::arrange(const core::rc32f & rect, const core::si32f & size)
    {
        setShowRect({ rect.pos, size });
    }

    void Control::updateStyle()
    {
        std::string style = styleName();
        if (style != _style)
        {
            auto ss = styleSheet();
            auto s = scene();
            std::map<std::string, std::string> items = ss->generate(style);
            auto iter_transition_duration = items.find("transition-duration");
            if (_style.empty() || !_styleTransition || !s || iter_transition_duration == items.end())
            {
                if (_animation)
                    _animation->clear();
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
                auto s = scene();
                auto a = animation();
                a->clear();
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

                        auto pa = std::make_shared<core::property_animation>(shared_from_this(), accessor, interpolator);
                        pa->setDuration(duration);
                        a->add(pa);
                    }
                    else
                    {
                        iter->second->set(item.second, *this);
                    }
                }

                a->start();
                s->start(a);
                
            }
            _style = style;
            invalid();
        }
    }

    void Control::update()
    {
        _invalid = false;
        auto s = scene();
        if (!s)
            return;

        std::lock_guard<std::mutex> lock(_mtx);
        _updateBackground();
        updateContent();
        _updateBorder();

        if (!_rect_invalid.empty() && s)
            s->invalid(_rect_invalid);
        _rect_invalid.clear();
    }

    void Control::onPosChanged(const core::pt32f & from, const core::pt32f & to)
    {
        invalid();
        posChanged(from, to);
    }
    void Control::onSizeChanged(const core::si32f & from, const core::si32f & to)
    {
        update();
        sizeChanged(from, to);
    }
    void Control::onRectChanged(const core::rc32f & from, const core::rc32f & to) { rectChanged(from, to); }

    void Control::onVisibleChanged(bool vis)
    {
        auto c = share_ref<Control>();
        auto s = scene();
        if (c && s)
            vis ? s->insert(c) : s->remove(c);
    }

    void Control::_updateBackground()
    {
        if (_background_image)
        {
            if (_background_rect_obj)
            {
                remove(_background_rect_obj);
                _background_rect_obj = nullptr;
            }

            if(!_background_imgage_obj)
            {
                _background_imgage_obj = std::make_shared<renderables::Image>(ref(), _background_image.value);
                insert(DEPTH_BACKGROUND, _background_imgage_obj);
            }

            _background_imgage_obj->setRect(box(_background_box));
            if (_background_size.available())
                _background_imgage_obj->setImageSize(calc(_background_size));
            _background_imgage_obj->setImageFitting(_background_fitting);
        }
        else if (_background_color)
        {
            if (_background_imgage_obj)
            {
                remove(_background_imgage_obj);
                _background_imgage_obj = nullptr;
            }
            if(!_background_rect_obj)
            {
                _background_rect_obj = std::make_shared<renderables::Rectangle>(ref());
                insert(DEPTH_BACKGROUND, _background_rect_obj);
            }

            _background_rect_obj->setRect(box(_background_box));
            _background_rect_obj->setRectangle(box(_background_box));
            _background_rect_obj->setPathStyle(graphics::PathStyle().fill(_background_color));
        }
        else
        {
            if (_background_imgage_obj)
            {
                remove(_background_imgage_obj);
                _background_imgage_obj = nullptr;
            }
            if (_background_rect_obj)
            {
                remove(_background_rect_obj);
                _background_rect_obj = nullptr;
            }
        }
    }

    void Control::_updateBorder()
    {
        if (_border && _border_colors)
        {
            if(std::equal(_border.value.arr.begin() + 1, _border.value.arr.end(), _border.value.arr.begin()) &&
                std::equal(_border_colors.value.arr.begin() + 1, _border_colors.value.arr.end(), _border_colors.value.arr.begin()) &&
                std::equal(_border_styles.value.arr.begin() + 1, _border_styles.value.arr.end(), _border_styles.value.arr.begin()))
            {
                if (!_border_obj)
                {
                    _border_obj = std::make_shared<renderables::Rectangle>(ref());
                    insert(DEPTH_FOREGROUND, _border_obj);
                }

                for (int32_t cnt = 0; cnt < 4; ++cnt)
                {
                    auto & border_obj = _border_objs[cnt];
                    if (border_obj)
                    {
                        remove(border_obj);
                        border_obj = nullptr;
                    }
                }
                _border_obj->setRect(box());
                _border_obj->setRectangle(box().expanded(calc(_border) * -0.5f));
                _border_obj->setPathStyle(graphics::PathStyle().stoke(_border_colors.value.x, _border_styles.value[0]).width(calc_x(_border.value.x)));
            }
            else
            {
                if (_border_obj)
                {
                    remove(_border_obj);
                    _border_obj = nullptr;
                }

                auto border = calc(_border);
                const core::align edges[4] = { core::align::left, core::align::top, core::align::right, core::align::bottom };
                for(int32_t cnt = 0; cnt < 4; ++cnt)
                {
                    auto & border_obj = _border_objs[cnt];
                    if (border[cnt] > 0 && _border_colors.value[cnt].visible())
                    {
                        auto path = std::make_shared<graphics::Path>();
                        auto points = boderPoints(edges[cnt]);
                        auto line = boderLine(edges[cnt]);
                        path->fromPoints(std::begin(points), std::end(points), true);

                        if (!border_obj)
                        {
                            border_obj = std::make_shared<renderables::Line>(ref(), line[0], line[1]);
                            insert(DEPTH_FOREGROUND, border_obj);
                        }
                        border_obj->setRect(path->computeTightBounds());
                        border_obj->setClipPath(path);
                        border_obj->setPathStyle(graphics::PathStyle().stoke(_border_colors.value[cnt], _border_styles.value[cnt]).width(border.arr[cnt]));
                    }
                    else
                    {
                        if (border_obj)
                        {
                            remove(border_obj);
                            border_obj = nullptr;
                        }
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

    void Control::invalid_rect(const core::rc32f & rect)
    {
        _rect_invalid.unite(rect);
    }

    void Control::insert(int32_t depth, std::shared_ptr<component::Component> object)
    {
        assert(object);
        if (!object)
            throw core::exception(core::error_nullptr);

        switch (object->type())
        {
        case ui::component::ComponentType::Renderable:
            _renderables.insert(std::make_pair(depth, std::dynamic_pointer_cast<component::Renderable>(object)));
            invalid_rect(object->rect());
            break;
        case ui::component::ComponentType::Interactable:
            _mouseareas.push_back(std::dynamic_pointer_cast<component::MouseArea>(object));
            break;
        default:
            break;
        }
    }

    void Control::remove(std::shared_ptr<component::Component> object)
    {
        switch (object->type())
        {
        case ui::component::ComponentType::Renderable:
            for (auto iter = _renderables.begin(); iter != _renderables.end(); ++iter)
            {
                if (iter->second == std::dynamic_pointer_cast<component::Renderable>(object))
                {
                    _renderables.erase(iter);
                    break;
                }
            }
            invalid_rect(object->rect());
            break;
        case ui::component::ComponentType::Interactable:
            break;
        default:
            break;
        }
    }

    void Control::clear()
    {
        _renderables.clear();
    }

    void Control::render(graphics::Graphics & graphics, const graphics::Region & region) const
    {
        std::lock_guard<std::mutex> lock(const_cast<Control *>(this)->_mtx);
        for (auto & rendereable : _renderables)
        {
            if (region.intersects(rendereable.second->rect().ceil<int32_t>()))
                rendereable.second->render(graphics);
        }
    }

    std::shared_ptr<component::MouseArea> Control::findMouseArea(const core::pt32f & pos, std::shared_ptr<component::MouseArea> last) const
    {
        bool found = false;
        for (auto iter = _mouseareas.rbegin(); iter != _mouseareas.rend(); ++iter)
        {
            if (last && !found)
            {
                if (*iter == last)
                    found = true;
                continue;
            }

            if ((*iter)->onHitTest(pos) == core::error_ok)
                return *iter;
        }
        return nullptr;
    }
}
