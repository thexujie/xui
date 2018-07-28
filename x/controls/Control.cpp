#include "stdafx.h"
#include "Control.h"
#include "renderables/Image.h"
#include "renderables/Rectangle.h"

namespace controls
{
    Control::Control() { }
    Control::~Control() { }

    core::si32f Control::prefferSize() const
    {
        // 如果设置了固定大小，直接返回即可
        if (_size.aviliable() && _size.value.cx.avi() && _size.value.cy.avi())
            return calc(_size.value);

        core::si32f size = contentSize() + calc(_padding).bsize();
        if (_size.aviliable())
        {
            if (_size.value.cx.avi())
                size.cx = calc(_size.value.cx);
            else if (_size.value.cy.avi())
                size.cy = calc(_size.value.cy);
            else {}
        }

        _adjustSizeMinMax(size);
        return size;
    }

    std::shared_ptr<View> Control::view() const
    {
        if (!_view)
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

    float32_t Control::calc(const core::dimensionf & value) const
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

    core::vec2f Control::calc(const core::vec2<core::dimensionf> & value) const
    {
        return { calc(value.x), calc(value.y) };
    }

    core::vec4f Control::calc(const core::vec4<core::dimensionf> & value) const
    {
        return { calc(value.x), calc(value.y), calc(value.cx), calc(value.cy) };
    }

    core::rc32f Control::borderBox() const
    {
        return core::rc32f(_rect.pos + _view_border.bleftTop(), _rect.size + _view_border.bsize());
    }

    core::rc32f Control::paddingBox() const
    {
        return core::rc32f(_rect.pos, _rect.size);
    }

    core::rc32f Control::contentBox() const
    {
        return core::rc32f(_rect.pos + _view_padding.bleftTop(), _rect.size - _view_padding.bsize());
    }

    core::rc32f Control::box(control_box box) const
    {
        switch (box)
        {
        case control_box::border_box: return borderBox();
        case control_box::padding_box: return paddingBox();
        case control_box::content_box: return contentBox();
        default: return borderBox();
        }
    }

    void Control::invalid() { }

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
        _scene = scene;
        scene->addRenderable(view());
    }

    void Control::enterScene(std::shared_ptr<component::Scene> & scene)
    {
        _view_border.bleft = calc(_border_left.width);
        _view_border.btop = calc(_border_top.width);
        _view_border.bright = calc(_border_right.width);
        _view_border.bbottom = calc(_border_bottom.width);
        _view_margin = calc(_margin);
        _view_padding = calc(_padding);
    }

    void Control::leavingScene(std::shared_ptr<component::Scene> & scene)
    {
        if (_view)
            scene->removeRenderable(_view);
        _scene.reset();
    }

    void Control::leaveScene(std::shared_ptr<component::Scene> & scene) { }

    void Control::layout(const core::rc32f & rect, const core::si32f & size)
    {
        auto p = parent();
        _rect.pos = rect.pos;
        _rect.size = size;
        //view()->setTransform(core::float3x2::translate(_rect.x, _rect.y));
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
        // 背景
        if (_background_image)
        {
            auto image = std::make_shared<renderables::Image>(_background_image);
            image->setRect(box(_background_box));
            if (_background_size.aviliable())
                image->setImageSize(calc(_background_size));
            image->setImageFitting(_background_fitting);
            v->insert(0, image);
        }
        else if (_background_color.visible())
        {
            auto rectangle = std::make_shared<renderables::Rectangle>();
            rectangle->setRect(box(_background_box));
            rectangle->setPathStyle(graphics::PathStyle().fill(_background_color));
            v->insert(0, rectangle);
        }
        else {}

        // 边框
        if(_border_colors)
        {
            auto rectangle = std::make_shared<renderables::Rectangle>();
            rectangle->setRect(borderBox());
            rectangle->setPathStyle(graphics::PathStyle().stoke(_border_colors.value.x));
            v->insert(0, rectangle);
        }
    }

    void Control::_adjustSizeMinMax(core::si32f & size) const
    {
        if (_minSize.aviliable())
        {
            if (_minSize.value.cx.avi())
            {
                float32_t val = calc(_minSize.value.cx);
                if (size.cx < val)
                    size.cx = val;
            }
            if (_minSize.value.cy.avi())
            {
                float32_t val = calc(_minSize.value.cy);
                if (size.cy < val)
                    size.cy = val;
            }
        }

        if (_maxSize.aviliable())
        {
            if (_maxSize.value.cx.avi())
            {
                float32_t val = calc(_maxSize.value.cx);
                if (size.cx > val)
                    size.cx = val;
            }
            if (_maxSize.value.cy.avi())
            {
                float32_t val = calc(_maxSize.value.cy);
                if (size.cy > val)
                    size.cy = val;
            }
        }
    }
}
