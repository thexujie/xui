#include "stdafx.h"
#include "Control.h"
#include "renderables/Image.h"
#include "renderables/Rectangle.h"

namespace controls
{
    Control::Control()
    {
        
    }
    Control::~Control()
    {
        
    }

    void Control::setMinSize(const core::unit_value<core::si32f> & minSize)
    {
        _minSize = minSize;
    }

    void Control::setMaxSize(const core::unit_value<core::si32f> & minSize)
    {
        _maxSize = minSize;
    }

    std::shared_ptr<View> Control::view() const
    {
        if(!_view)
            const_cast<std::shared_ptr<View> &>(_view) = std::make_shared<View>(const_cast<Control *>(this)->shared_from_this());
        return _view;
    }

    const core::color32 & Control::color() const
    {
        if (_color.aviliable())
            return _color;

        auto p = parent();
        if (p)
            return p->color();
        else
            return _color;
    }

    const graphics::font & Control::font() const
    {
        if (_font.aviliable())
            return _font;

        auto p = parent();
        if (p)
            return p->font();
        else
            return _font;
    }

    float32_t Control::calc(const core::unit_value<float32_t> & value) const
    {
        auto s = scene();
        if (!s)
            return {};

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
        default:
            return value.value * s->ratio();
        }
    }

    core::vec2f Control::calc(const core::vec2<core::unit_value<float32_t>> & value) const
    {
        return { calc(value.x), calc(value.y) };
    }

    core::vec4f Control::calc(const core::vec4<core::unit_value<float32_t>> & value) const
    {
        return { calc(value.x), calc(value.y), calc(value.cx), calc(value.cy) };
    }

    float32_t Control::map(const core::unit_value<float32_t> & value, float32_t ref) const
    {
        auto s = scene();
        if (!s)
            return {};

        switch (value.unit)
        {
        case core::unit::px:
            return value.value * s->ratio();
            break;
        case core::unit::em:
            return value.value * font().size * s->ratio();
            break;
        case core::unit::pt:
            return value.value * 72.0f * s->ratio();
            break;
        case core::unit::per:
            return value.value * ref;
            break;
        case core::unit::dot:
            return value.value;
            break;
        default:
            return value.value * s->ratio();
        }
    }

    core::vec2<float32_t> Control::map(const core::vec2<core::unit_value<float32_t>> & size) const
    {
        auto s = scene();
        auto p = parent();
        if (!s || !p)
            return {};

        return { map(size.x, p->width()), map(size.y, p->height()) };
    }

    core::vec4<float32_t> Control::map(const core::vec4<core::unit_value<float32_t>> & rect) const
    {
        auto s = scene();
        auto p = parent();
        if (!s || !p)
            return {};
        return { map(rect.x, p->width()), map(rect.y, p->height()), map(rect.cx, p->height()), map(rect.cy, p->height()) };
    }

    core::rc32f Control::borderBox() const
    {
        return core::rc32f(_view_border.leftTop(), _view_rect.size - _view_border.bsize());
    }

    core::rc32f Control::paddingBox() const
    {
        return core::rc32f({}, _view_rect.size);
    }

    core::rc32f Control::contentBox() const
    {
        return core::rc32f(_view_rect.pos, _view_rect.size);
    }

    core::rc32f Control::box(control_box box) const
    {
        switch(box)
        {
        case control_box::border_box: return borderBox();
        case control_box::padding_box: return paddingBox();
        case control_box::content_box: return contentBox();
        default: return borderBox();
        }
    }

    void Control::invalid()
    {
        
    }

    void Control::setBackgroundColor(core::color32 color)
    {
        if (color == _background_color)
            return;

        _background_color = color;
        if (!_background_image)
            invalid();
    }

    core::color32 Control::backgroundColor() const
    {
        return _background_color;
    }

    void Control::setBackgroundImage(std::shared_ptr<graphics::Image> image)
    {
        if (image == _background_image)
            return;
        _background_image = image;
        invalid();
    }

    std::shared_ptr<graphics::Image> Control::backgroundImage() const
    {
        return _background_image;
    }

    void Control::enteringScene(std::shared_ptr<component::Scene> & scene)
    {
        scene->addRenderable(view());
    }

    void Control::enterScene(std::shared_ptr<component::Scene> & scene)
    {
        _scene = scene;
        _view_border.bleft = calc(_border_left.width);
        _view_border.btop = calc(_border_top.width);
        _view_border.bright = calc(_border_right.width);
        _view_border.bbottom = calc(_border_bottom.width);
        _view_margin = calc(_margin);
        _view_padding = calc(_paddding);
    }

    void Control::leavingScene(std::shared_ptr<component::Scene> & scene)
    {
        if(_view)
            scene->removeRenderable(_view);
    }

    void Control::leaveScene(std::shared_ptr<component::Scene> & scene)
    {
        _scene.reset();
    }

    void Control::layout(const core::rc32f & rect)
    {
        auto p = parent();
        if(rect.empty())
        {
            if (_size.aviliable())
                _view_rect.size = { map(_size.value.x, p ? p->width() : 0), map(_size.value.y, p ? p->height() : 0) };
            else
                _view_rect.size = contentSize() + _view_padding.bsize() + _view_border.bsize();
        }
        else
        {
            switch (_layout_origin)
            {
            case position_origin::layout:
                _view_rect.pos = rect.pos;
                break;
            case position_origin::absolute:
                _view_rect.pos = p ? p->map(_position.value) : core::vec2f();
                break;
            case position_origin::fixed:
                _view_rect.pos = p ? p->map(_position.value) : core::vec2f();
                break;
            case position_origin::relative:
                _view_rect.pos = rect.pos + map(_position.value);
            case position_origin::sticky:
                break;
            default:
                break;
            }
        }
    }

    void Control::update()
    {
        _view->clear();
        _updateBackground();
    }

    void Control::onRectChanged(const core::rc32f & from, const core::rc32f & to)
    {
        rectChanged(from, to);
    }

    void Control::_updateBackground() const
    {
        auto v = view();
        if(_background_image)
        {
            auto image = std::make_shared<renderables::Image>(_background_image);
            image->setRect(box(_background_origin));
            if(_background_size.aviliable())
                image->setImageSize(map(_background_size));
            image->setRepeat(_background_repeat);
            v->insert(0, image);
        }
        else if(_background_color.visible())
        {
            auto rectangle = std::make_shared<renderables::Rectangle>();
            rectangle->setRect(box(_background_origin));
            rectangle->setPathStyle(graphics::PathStyle().fill(_background_color));
            v->insert(0, rectangle);
        }
        else {}
    }
}
