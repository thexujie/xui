#pragma once
#include "component/Renderable.h"
#include "drawing/Region.h"
#include "component/Style.h"

namespace ui
{
    class ImeContext;
    enum class mouse_action;
    class mosue_state;

    class Scene : public core::object
    {
    public:
        Scene(std::shared_ptr<Control> control);
        ~Scene();

    public:
        std::shared_ptr<Control> control() const { return _control.lock(); }
        float32_t ratio() const { return _ratio; }
        const core::rc32f & rect() const { return _rect; }
        const core::si32f & size() const { return _rect.size; }
        const core::rc32f & viewRect() const { return _rect; }

        void setStyleSheet(std::shared_ptr<component::StyleSheet> styleSheet) { _styleSheet = styleSheet;  }
        std::shared_ptr<component::StyleSheet> styleSheet() const { return _styleSheet; }

        void setImeContext(std::shared_ptr<ui::ImeContext> imecontext) { _imecontext = imecontext; }
        std::shared_ptr<ui::ImeContext> imeContext() const { return _imecontext; }


        std::shared_ptr<drawing::Bitmap> bitmap() const { return _renderBuffer; }
        void invalid(const core::rc32f & rect);
        const core::rc32i & invalidRect() const { return _invalid_rect; }

    public:
        core::error animate();

    public:
        void onMouseState(const mosue_state & state, mouse_action action);
        void onChar(char32_t ch);

    private:
        void _updateMouseArea(const mosue_state & state, mouse_action action);

    public:
        core::event<void(const core::rc32i & rect)> invalidated;
        core::event<void(const drawing::Region & region)> rendered;
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
        drawing::Region _invalid_region;
        std::weak_ptr<Control> _control;
        core::color32 _color_default = core::colors::AliceBlue;
        float32_t _ratio = 1.0f;
        core::rc32f _rect;

        std::list<std::shared_ptr<component::Renderable>> _renderables;

        std::shared_ptr<drawing::Bitmap> _renderBuffer;

        std::shared_ptr<Control> _interactbale_current;
        std::shared_ptr<Control> _interactbale_input;

        std::shared_ptr<component::StyleSheet> _styleSheet;

        core::timer _animation_timer;

        std::shared_ptr<ui::ImeContext> _imecontext;
    };
}
