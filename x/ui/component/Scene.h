#pragma once
#include "Renderable.h"
#include "Interactable.h"
#include "graphics/Region.h"
#include "Animation.h"

namespace ui
{
    class Control;
}

namespace ui::component
{
    class Component;
    class StyleSheet;

    class Scene : public core::object
    {
    public:
        Scene(std::shared_ptr<Control> control);
        ~Scene();

    public:
        float32_t ratio() const { return _ratio; }
        const core::rc32f & rect() const { return _rect; }
        const core::si32f & size() const { return _rect.size; }
        const core::rc32f & viewRect() const { return _rect; }

        void setStyleSheet(std::shared_ptr<component::StyleSheet> styleSheet) { _styleSheet = styleSheet;  }
        std::shared_ptr<component::StyleSheet> styleSheet() const { return _styleSheet; }

        std::shared_ptr<graphics::Bitmap> bitmap() const { return _renderBuffer; }
        void invalid(const core::rc32f & rect);
        void update();
        void flush();
        const core::rc32i & invalidRect() const { return _invalid_rect; }

    public:
        core::error insert(std::shared_ptr<Control> control);
        core::error remove(std::shared_ptr<Control> control);

        core::error start(std::shared_ptr<Animation> animation);

        const std::list<std::shared_ptr<Control>> & views() const { return _controls; }
    public:
        std::shared_ptr<MouseArea> findMouseArea(const core::pt32f & pos, std::shared_ptr<MouseArea> last = nullptr) const;

        virtual void onMouseEnter(const mosue_state & state);
        virtual void onMouseMove(const mosue_state & state);
        virtual void onMouseLeave(const mosue_state & state);

        virtual void onMouseDown(const mosue_state & state);
        virtual void onMouseUp(const mosue_state & state);
        virtual void onMouseClick(const mosue_state & state);
        virtual void onMouseDBClick(const mosue_state & state);
        virtual void onMouseState(const mosue_state & state);

    private:
        void _updateMouseArea(const mosue_state & state);

    public:
        core::event<void(const core::rc32i & rect)> invalidated;
        //core::event<void(const core::rc32i & rect)> rendered;
        core::event<void(const graphics::Region & region)> rendered;
        core::event<void(bool capture)> captured;

    private:
        void renderThread();
        void animationTimerTick(core::timer & t, int64_t tick);
    protected:
        std::mutex _mtx;
        std::condition_variable _cv_render;
        std::thread _th_render;
        std::atomic_bool _exit = false;

        core::rc32i _invalid_rect;
        graphics::Region _invalid_region;
        std::weak_ptr<Control> _control;
        core::color32 _color_default = core::colors::AliceBlue;
        float32_t _ratio = 1.0f;
        core::rc32f _rect;
        std::list<std::shared_ptr<Control>> _controls;
        std::list<std::shared_ptr<Animation>> _animations;

        std::list<std::shared_ptr<Renderable>> _renderables;

        std::shared_ptr<graphics::Bitmap> _renderBuffer;

        std::shared_ptr<MouseArea> _mousearea_current;

        std::shared_ptr<component::StyleSheet> _styleSheet;

        core::timer _animation_timer;

        std::future<void> _future;
    };
}
