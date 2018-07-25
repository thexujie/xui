#include "stdafx.h"
#include "Rectangle.h"

namespace controls
{
    Rectangle::Rectangle():_rectangle(std::make_shared<renderables::Rectangle>())
    {
        
    }

    Rectangle::Rectangle(const core::rc32f & rect) : _rectangle(std::make_shared<renderables::Rectangle>(rect))
    {

    }

    Rectangle::~Rectangle()
    {

    }

    void Rectangle::setPathStyle(const graphics::PathStyle & style)
    {
        if (style == _style)
            return;

        _style = style;
        _rectangle->setPathStyle(_style);
    }

    void Rectangle::enteringScene(std::shared_ptr<component::Scene> & scene)
    {
        _rectangle->setPos(_rect.center());
        _rectangle->setSize(_rect.size);
        View::enteringScene(scene);
    }

    void Rectangle::enterScene(std::shared_ptr<component::Scene> & scene)
    {
        scene->addRenderable(_rectangle);
        View::enterScene(scene);
    }

    void Rectangle::leaveScene(std::shared_ptr<component::Scene> & scene)
    {
        scene->removeRenderable(_rectangle);
        View::leaveScene(scene);
    }

    void Rectangle::onRectChanged(const core::rc32f & from, const core::rc32f & to)
    {
        _rectangle->setPos(_rect.center());
        _rectangle->setSize(_rect.size);
        View::onRectChanged(from, to);
    }
}
