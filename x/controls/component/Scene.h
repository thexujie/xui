#pragma once
#include "Renderable.h"
#include "Interactable.h"

namespace controls::component
{
    class Component;

    class Scene
    {
    public:
        Scene();
        ~Scene();

    public:
        float32_t ratio() const { return _ratio; }
        const core::rc32f & rect() const { return _rect; }
        const core::si32f & size() const { return _rect.size; }
        const core::rc32f & viewRect() const { return _rect; }

    public:
        core::error addRenderable(std::shared_ptr<Renderable> component);
        core::error removeRenderable(std::shared_ptr<Renderable> component);

        const std::list<std::shared_ptr<Renderable>> & renderables() const { return _renderables; }
        const std::list<std::shared_ptr<Interactable>> & interactables() const { return _interactables; }

        void render(graphics::Graphics & graphics) const;

    protected:
        float32_t _ratio = 1.0f;
        core::rc32f _rect;
        std::list<std::shared_ptr<Renderable>> _renderables;
        std::list<std::shared_ptr<Interactable>> _interactables;
    };
}
