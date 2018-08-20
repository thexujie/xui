#pragma once

namespace graphics {
    class Graphics;
}

namespace controls::component
{
    class Scene;
    class View;

    enum class ComponentType
    {
        Renderable = 0,
        Interactable,
    };

    class Component : public core::object
    {
    public:
        Component(ComponentType type);
        Component(ComponentType type, const core::rc32f & rect);

        virtual ~Component() = default;

        ComponentType type() const { return _type; }

        void setPos(const core::pt32f & pos) { if (_rect.pos != pos) { invalid(); _rect.pos = pos; invalid(); }}
        core::pt32f pos() const { return _rect.pos; }
        void setSize(const core::si32f & size) { if (_rect.size != size) { invalid(); _rect.size = size; invalid(); } }
        core::si32f size() const { return _rect.size; }
        void setRect(const core::rc32f & rect) { if (_rect != rect) { invalid(); _rect = rect; invalid(); } }
        core::rc32f rect() const { return _rect; }

        std::shared_ptr<View> view() const { return _view.lock(); }

        void invalid();
    public:
        virtual void enteringScene(Scene & scene);
        virtual void enterScene(Scene & scene);
        virtual void leavingScene(Scene & scene);
        virtual void leaveScene(Scene & scene);

        virtual void enterView(std::shared_ptr<View> view) { _view = view; }
        virtual void leaveView(std::shared_ptr<View> view) { _view.reset(); }

    protected:
        ComponentType _type;
        core::rc32f _rect;
        std::weak_ptr<View> _view;
    };
}
