#include "stdafx.h"
#include "Scene.h"
#include "Component.h"

namespace controls::component
{
    Scene::Scene()
    {

    }

    Scene::~Scene()
    {

    }

    core::error Scene::addComponent(std::shared_ptr<Component> component)
    {
        if (!component)
            return core::error_args;

        component->enteringScene(*this);
        switch (component->type())
        {
        case ComponentType::Renderable:
            _renderables.push_back(std::dynamic_pointer_cast<Renderable>(component));
            break;
        default:
            break;
        }
        component->enterScene(*this);
        return core::error_ok;
    }

    core::error Scene::removeComponent(std::shared_ptr<Component> component)
    {
        if (!component)
            return core::error_args;

        component->leavingScene(*this);
        switch (component->type())
        {
        case ComponentType::Renderable:
            _renderables.remove(std::dynamic_pointer_cast<Renderable>(component));
            break;
        default:
            break;
        }
        component->leaveScene(*this);
        return core::error_ok;
    }

    void Scene::render(graphics::Graphics & graphics) const
    {
        for (auto & renderable : _renderables)
            renderable->render(graphics);
    }
}
