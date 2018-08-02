#pragma once
#include "Renderable.h"
#include "Interactable.h"

namespace controls::component
{
    class Component;

    class Scene : public core::invokable<Scene>
    {
    public:
        Scene();
        ~Scene();

    public:
        float32_t ratio() const { return _ratio; }
        const core::rc32f & rect() const { return _rect; }
        const core::si32f & size() const { return _rect.size; }
        const core::rc32f & viewRect() const { return _rect; }

        std::shared_ptr<graphics::Bitmap> bitmap() const { return _renderBuffer; }
        void invalid(const core::rc32f & rect);
        void flush();
        const core::rc32f & invalidRect() const { return _invalid_rect; }

    public:
        core::error insert(std::shared_ptr<View> component);
        core::error remove(std::shared_ptr<View> component);

        const std::list<std::shared_ptr<View>> & views() const { return _views; }

        void render(graphics::Graphics & graphics) const;

    public:
        std::shared_ptr<MouseArea> findMouseArea(const core::pt32f & pos) const;

        virtual void onMouseEnter(const mosue_state & state);
        virtual void onMouseMove(const mosue_state & state);
        virtual void onMouseLeave(const mosue_state & state);

        virtual void onMouseDown(const mosue_state & state);
        virtual void onMouseUp(const mosue_state & state);
        virtual void onMouseClick(const mosue_state & state);
        virtual void onMouseDBClick(const mosue_state & state);

    public:
        core::event<void(const core::rc32f & rect)> invalidated;
        core::event<void(const core::rc32f & rect)> rendered;
        
    protected:
        bool _invalid = false;
        core::rc32f _invalid_rect;
        core::color32 _color_default = core::colors::AliceBlue;
        float32_t _ratio = 1.0f;
        core::rc32f _rect;
        std::list<std::shared_ptr<View>> _views;

        std::shared_ptr<graphics::Bitmap> _renderBuffer;

        std::shared_ptr<MouseArea> _mousearea_hoving;
    };
}
