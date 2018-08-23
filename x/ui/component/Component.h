#pragma once

namespace ui {
    class Control;
}

namespace graphics {
    class Graphics;
}

namespace ui::component
{
    class Scene;
    class View;

    enum class ComponentType
    {
        Control = 0,
        Renderable,
        Interactable,
    };

    class Component : public core::object
    {
    public:
        Component(ComponentType type, std::shared_ptr<ui::Control> control);
        Component(ComponentType type, std::shared_ptr<ui::Control> control, const core::rc32f & rect);

        virtual ~Component() = default;

        ComponentType type() const { return _type; }

        void setRect(const core::rc32f & rect) { if (_rect != rect) { auto old_rect = _rect; _rect = rect; onRectChanged(old_rect, rect); } }
        core::rc32f rect() const { return _rect; }

        std::shared_ptr<ui::Control> control() const { return _control.lock(); }

        virtual void onRectChanged(const core::rc32f & from, const core::rc32f & to) {}

    protected:
        ComponentType _type;
        std::weak_ptr<ui::Control> _control;
        core::rc32f _rect;
    };
}
