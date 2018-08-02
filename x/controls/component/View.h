#pragma once
#include "Component.h"
#include "Renderable.h"
#include "Interactable.h"

namespace controls::component
{
    class View :public Component
    {
    public:
        View() : Component(ComponentType::Renderable) {}
        virtual ~View() = default;

        void lock();
        void unlock();
        void invalid(const core::rc32f & rect);

        std::shared_ptr<Scene> scene() const { return _scene.lock(); }
        virtual void enteringScene(std::shared_ptr<Scene> scene);
        virtual void enterScene(std::shared_ptr<Scene> scene);
        virtual void leavingScene(std::shared_ptr<Scene> scene);
        virtual void leaveScene(std::shared_ptr<Scene> scene);

        void clear();
        void insert(std::shared_ptr<Component> object);
        void remove(std::shared_ptr<Component> object);

        virtual void render(graphics::Graphics & graphics) const;

        std::shared_ptr<MouseArea> findMouseArea(const core::pt32f & pos) const;

        void setTransform(const core::float3x2 & transform) { _transform = transform; }

    protected:
        std::mutex _mtx;
        std::mutex _mtx_interactable;
        std::weak_ptr<Scene> _scene;
        core::rc32f _rect;
        core::float3x2 _transform;
        std::list<std::shared_ptr<Renderable>> _renderables;
        std::list<std::shared_ptr<MouseArea>> _mouseareas;

        core::rc32f _rect_invalid;
    };
}
