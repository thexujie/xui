#include "stdafx.h"
#include "View.h"
#include "Scene.h"
#include "Renderable.h"

namespace controls::component
{
    void View::lock()
    {
        _mtx.lock();
    }

    void View::unlock()
    {
        auto s = scene();
        if(!_rect_invalid.empty() && s)
        {
            s->invalid(_rect_invalid);
            _rect_invalid.clear();
        }

        _mtx.unlock();
    }

    void View::invalid(const core::rc32f & rect)
    {
        _rect_invalid = _rect_invalid.intersect(rect);
    }

    void View::enteringScene(std::shared_ptr<component::Scene> scene)
    {

    }

    void View::enterScene(std::shared_ptr<component::Scene> scene)
    {
        _scene = scene;
    }

    void View::leavingScene(std::shared_ptr<component::Scene> scene)
    {

    }

    void View::leaveScene(std::shared_ptr<component::Scene> scene)
    {
        _scene.reset();
    }


    void View::insert(std::shared_ptr<controls::component::Component> object)
    {
        switch(object->type())
        {
        case ComponentType::Renderable:
            _renderables.push_back(std::dynamic_pointer_cast<Renderable>(object));
            invalid(object->rect());
            break;
        case ComponentType::Interactable: 
            break;
        default: 
            break;
        }
    }

    void View::remove(std::shared_ptr<Component> object)
    {
        switch (object->type())
        {
        case ComponentType::Renderable:
            _renderables.remove(std::dynamic_pointer_cast<Renderable>(object));
            invalid(object->rect());
            break;
        case ComponentType::Interactable:
            break;
        default:
            break;
        }
    }

    void View::clear()
    {
        _renderables.clear();
    }

    void View::render(graphics::Graphics & graphics) const
    {
        std::lock_guard<View> lock(const_cast<View &>(*this));
        graphics.setMatrix(_transform);
        for(auto & rendereable : _renderables)
            rendereable->render(graphics);
        graphics.setMatrix(core::float3x2::identity);
    }
}
