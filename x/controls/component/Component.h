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
        Component(ComponentType type, const core::pt32f & pos) : _type(type), _pos(pos) {}

        virtual ~Component() = default;

        ComponentType type() const { return _type; }
        void setPos(const core::pt32f & pt);
        core::pt32f pos() const;

    protected:
        virtual void enteringScene(Scene & scene);
        virtual void enterScene(Scene & scene);
        virtual void leavingScene(Scene & scene);
        virtual void leaveScene(Scene & scene);

    protected:
        ComponentType _type;
        core::pt32f _pos;
    };
}
