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
        core::error addRenderable(std::shared_ptr<Renderable> component);
        core::error removeRenderable(std::shared_ptr<Renderable> component);

        const std::list<std::shared_ptr<Renderable>> & renderables() const { return _renderables; }
        const std::list<std::shared_ptr<Interactable>> & interactables() const { return _interactables; }

        void render(graphics::Graphics & graphics) const;

    protected:
        std::list<std::shared_ptr<Renderable>> _renderables;
        std::list<std::shared_ptr<Interactable>> _interactables;
    };
}
