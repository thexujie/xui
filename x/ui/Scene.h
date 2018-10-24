#pragma once
#include "drawing/Region.h"
#include "component/Style.h"
#include "UICommon.h"

namespace ui
{
    class Control;
    class RadioGroup;

    enum class scene_event
    {
        none = 0,
        update_mouse_pos,
    };

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

        void setEvent(scene_event evt);
        std::shared_ptr<RadioGroup> radioGroup(std::string name);

        void setStyleSheet(std::shared_ptr<component::StyleSheet> styleSheet) { _style_sheet = styleSheet;  }
        std::shared_ptr<component::StyleSheet> styleSheet() const { return _style_sheet; }

        void setImeContext(std::shared_ptr<ui::ImeContext> ime_context) { _ime_context = ime_context; }
        std::shared_ptr<ui::ImeContext> imeContext() const { return _ime_context; }

        void setCursorContext(std::shared_ptr<CursorContext> cursor_context)  { _cursor_context = cursor_context; }
        std::shared_ptr<CursorContext> cursorContext() const { return _cursor_context; }

        std::shared_ptr<drawing::GraphicsDevice> bitmap() const { return _draw_buffer; }
        void invalidate(const core::rc32f & rect);

        std::shared_ptr<drawing::GraphicsDevice> readBegin();
        void readEnd();


    public:
        core::error animate();

    public:
        void onMouse(const input_state & state, mouse_button button, mouse_action action);
        void onKey(const input_state & state, keycode key,  key_action action);
        void onChar(char32_t ch);
		void onCaptured(bool c);


    private:
        void _updateMouseArea(const input_state & state, mouse_action action);

    public:
        core::event<void(const core::rc32i & rect)> invalidated;
        core::event<void(const core::rc32i & rect)> rendered;
        core::event<void(bool capture)> captured;
        core::event<void(scene_event evt)> evented;

    private:
        void renderThread();
        void animationTimerTick(core::timer & t, int64_t tick);

    protected:
        enum buffer_state
        {
            idle = 0,
            writing,
            reading,
        };

        std::mutex _mtx;
        std::condition_variable _cv_render;
        std::thread _th_render;
        std::atomic_bool _exit = false;

        drawing::Region _invalid_region;
        std::weak_ptr<Control> _control;
        core::color _color_default = core::colors::AliceBlue;
        float32_t _ratio = 1.0f;
        core::rc32f _rect;

		std::mutex _mtx_buffer;
        std::shared_ptr<drawing::Surface> _draw_buffer;

        std::shared_ptr<Control> _current_control;
		std::shared_ptr<Control> _current_input;
		std::shared_ptr<Control> _captured_control;

        std::shared_ptr<component::StyleSheet> _style_sheet;

        core::timer _animation_timer;

        std::shared_ptr<ui::ImeContext> _ime_context;
        std::shared_ptr<ui::CursorContext> _cursor_context;

        std::map<std::string, std::weak_ptr<RadioGroup>> _radio_groups;
    };
}
