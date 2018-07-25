#pragma once

namespace controls::component
{
    class Scene;

    enum class ComponentType
    {
        Renderable = 0,
        Interactable,
    };

    class Component : public std::enable_shared_from_this<Component>
    {
        friend class Scene;

        public:
        Component() = default;
        Component(ComponentType type):_type(type) {}
        Component(ComponentType type, const core::rc32f & rect) : _type(type), _rect(rect) {}

        virtual ~Component() = default;

        ComponentType type() const { return _type; }

        void setPos(const core::pt32f & pos) { _rect.position = pos; }
        core::pt32f pos() const { return _rect.position; }
        void setSize(const core::si32f & size) { _rect.size = size; }
        core::si32f size() const { return _rect.size; }
        void setRect(const core::rc32f & rect) { _rect = rect; }
        core::rc32f rect() const { return _rect; }

    protected:
        virtual void enteringScene(Scene & scene);
        virtual void enterScene(Scene & scene);
        virtual void leavingScene(Scene & scene);
        virtual void leaveScene(Scene & scene);

    protected:
        ComponentType _type;
        core::rc32f _rect;
    };
}
