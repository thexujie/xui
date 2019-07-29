#pragma once

namespace ui {
    class Control;
}

namespace drawing {
    class Graphics;
}

namespace ui::component
{
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
        Component(ComponentType type, std::shared_ptr<ui::Control> control, const core::rectf & rect);

        virtual ~Component() = default;

        ComponentType type() const { return _type; }

        void setVisible(bool vis) { if (_visible != vis) { _visible = vis;  onVisibleChanged(vis); } }
        bool visible() const { return _visible; }

        void setRect(const core::rectf & rect) { if (_rect != rect) { auto old_rect = _rect; _rect = rect; onRectChanged(old_rect, rect); } }
        core::rectf rect() const { return _rect; }

        std::shared_ptr<ui::Control> control() const { return _control.lock(); }

        virtual void onVisibleChanged(bool vis) {}
        virtual void onRectChanged(const core::rectf & from, const core::rectf & to) {}

    protected:
        ComponentType _type;
        std::weak_ptr<ui::Control> _control;
        core::rectf _rect;
        bool _visible = true;
    };
}
