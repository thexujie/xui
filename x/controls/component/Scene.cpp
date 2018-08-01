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

    void Scene::invalid(const core::rc32f & rect)
    {
        _invalid = true;
        _invalid_rect = _invalid_rect.unite(rect);
        invalidated(_invalid_rect);
        invoke([this]() {flush(); });
    }

    void Scene::flush()
    {
        if (_invalid_rect.empty())
            return;
        render();
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

    void Scene::render() const
    {
        if (_invalid_rect.empty())
            return;

        core::si32i size = _invalid_rect.size.to<int32_t>();
        if (!_renderBuffer || _renderBuffer->size().cx < size.cx || _renderBuffer->size().cy < size.cy)
            const_cast<Scene *>(this)->_renderBuffer = std::make_shared<graphics::Bitmap>(size);

        graphics::Graphics graphics(_renderBuffer);
        graphics.clear(_color_default);
        render(graphics);
        const_cast<Scene *>(this)->rendered(_invalid_rect);
        const_cast<Scene *>(this)->_invalid_rect.clear();
    }

    void Scene::render(graphics::Graphics & graphics) const
    {
        for (auto & renderable : _renderables)
            renderable->render(graphics);
        const_cast<Scene *>(this)->_invalid = false;
    }
}
