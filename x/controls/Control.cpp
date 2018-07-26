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

    void Control::setRect(const core::rc32f & rect)
    {
        if (_view_rect == rect)
            return;

        core::rc32f rect_old = _view_rect;
        _view_rect = rect;
        onRectChanged(rect_old, _view_rect);
    }

    void Control::setPos(const core::pt32f & pos)
    {
        if (_view_rect.pos == pos)
            return;
        core::rc32f rect_old = _view_rect;
        _view_rect.pos = pos;
        onRectChanged(rect_old, _view_rect);
    }

    void Control::setSize(const core::si32f & size)
    {
        if (_view_rect.size == size)
            return;
        core::rc32f rect_old = _view_rect;
        _view_rect.size = size;
        onRectChanged(rect_old, _view_rect);
    }

    void Control::setMinSize(const unit_value<core::si32f> & minSize)
    {
        _minSize = minSize;
    }

    void Control::setMaxSize(const unit_value<core::si32f> & minSize)
    {
        _maxSize = minSize;
    }

    core::si32f Control::prefferdSize() const
    {
        return core::si32f();
    }

    std::shared_ptr<View> Control::view() const
    {
        if(!_view)
            const_cast<std::shared_ptr<View> &>(_view) = std::make_shared<View>();
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

    float32_t Control::map(const unit_value<float32_t> & value, float32_t ref) const
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
        default:
            return value.value * s->ratio();
        }
    }

    core::vec2<float32_t> Control::map(const core::vec2<unit_value<float32_t>> & size) const
    {
        auto s = scene();
        auto p = parent();
        if (!s || !p)
            return {};

        return { map(size.x, p->width()), map(size.y, p->height()) };
    }

    core::vec4<float32_t> Control::map(const core::vec4<unit_value<float32_t>> & rect) const
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
        return core::rc32f({}, _view_rect.size);
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

    }

    void Control::enterScene(std::shared_ptr<component::Scene> & scene)
    {
        _scene = scene;
    }

    void Control::leavingScene(std::shared_ptr<component::Scene> & scene)
    {

    }

    void Control::leaveScene(std::shared_ptr<component::Scene> & scene)
    {
        _scene.reset();
    }

    void Control::layoutContent()
    {
        
    }

    void Control::layout(const core::pt32f & pos)
    {
        // size
        auto p = parent();
        if (!p)
            return;

        layoutContent();

        _view_border.bleft = map(_border_left.width, width());
        _view_border.btop = map(_border_top.width, height());
        _view_border.bright = map(_border_right.width, width());
        _view_border.bbottom = map(_border_bottom.width, height());

        if (_size.aviliable())
            _view_rect.size = { map(_size.value.x, p->width()), map(_size.value.y, p->height()) };
        else
            _view_rect.size = contentSize() + _view_border.bsize();

        // pos
        //auto pos = map(_position.value);
        switch(_layout_origin)
        {
        case position_origin::layout:
            _view_rect.pos = pos;
            break;
        case position_origin::absolute:
            _view_rect.pos = p->map(_position.value);
            break;
        case position_origin::fixed:
            _view_rect.pos = p->map(_position.value);
            break;
        case position_origin::relative:
            _view_rect.pos = pos + map(_position.value);
        case position_origin::sticky: 
            break;
        default: 
            break;
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
