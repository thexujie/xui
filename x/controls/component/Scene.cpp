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

    core::error Scene::addRenderable(std::shared_ptr<Renderable> component)
    {
        if (!component)
            return core::error_args;

        component->enteringScene(*this);
        _renderables.push_back(std::dynamic_pointer_cast<Renderable>(component));
        component->enterScene(*this);
        return core::error_ok;
    }

    core::error Scene::removeRenderable(std::shared_ptr<Renderable> component)
    {
        if (!component)
            return core::error_args;

        component->leavingScene(*this);
        _renderables.remove(component);
        component->leaveScene(*this);
        return core::error_ok;
    }

    void Scene::render(graphics::Graphics & graphics) const
    {
        for (auto & renderable : _renderables)
            renderable->render(graphics);
    }
}
