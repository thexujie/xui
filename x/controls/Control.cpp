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
        if (_rect == rect)
            return;

        core::rc32f rect_old = _rect;
        _rect = rect;
        onRectChanged(rect_old, _rect);
    }

    void Control::setPos(const core::pt32f & pos)
    {
        if (_rect.position == pos)
            return;
        core::rc32f rect_old = _rect;
        _rect.position = pos;
        onRectChanged(rect_old, _rect);
    }

    void Control::setSize(const core::si32f & size)
    {
        if (_rect.size == size)
            return;
        core::rc32f rect_old = _rect;
        _rect.size = size;
        onRectChanged(rect_old, _rect);
    }

    core::si32f Control::map(const unit_value<core::si32f> & value) const
    {
        return value.value;
    }

    std::shared_ptr<component::Scene> Control::scene() const
    {
        return _scene.lock();
    }

    const core::rc32f & Control::borderBox() const
    {
        return core::rc32f({}, _rect.size);
    }

    const core::rc32f & Control::paddingBox() const
    {
        return core::rc32f({}, _rect.size);
    }

    const core::rc32f & Control::contentBox() const
    {
        return core::rc32f({}, _rect.size);
    }

    const core::rc32f & Control::box(control_box box) const
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

    void Control::onUpdate(graphics::Graphics & graphics)
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
        if(_background_image)
        {
            auto image = std::make_shared<renderables::Image>(_background_image);
            image->setRect(box(_background_origin));
            if(_background_size)
                image->setImageSize(map(_background_size));
            image->setRepeat(_background_repeat);
            _view->insert(0, image);
        }
        else if(_background_color.visible())
        {
            auto rectangle = std::make_shared<renderables::Rectangle>();
            rectangle->setRect(box(_background_origin));
            rectangle->setPathStyle(graphics::PathStyle().fill(_background_color));
            _view->insert(0, rectangle);
        }
    }
}
