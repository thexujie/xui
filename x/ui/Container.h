#pragma once
#include "Control.h"

namespace ui
{
    namespace controls
    {
        class ScrollBar;
        class Spacer;
    }

    enum class scrollbar_vision
    {
        auto_hide = 0,
        disabled,
        always,
    };

    class Container : public Control
    {
    public:
        Container();
        Container(core::align layout);
        virtual ~Container();

        void addControl(std::shared_ptr<Control> control);
        std::shared_ptr<controls::Spacer> addSpacer(core::dimensionf size);
        void removeControl(std::shared_ptr<Control> control);
        std::vector<std::shared_ptr<Control>> & children() { return _controls; }

        void onEnteringScene(std::shared_ptr<Scene> & scene) override;
        void onEnterScene(std::shared_ptr<Scene> & scene) override;
        void onLeavingScene() override;
        void onLeaveScene() override;

        core::si32f contentSize() const override;
        void update() override;
        using Control::invalidate;
        void invalidate(const core::rc32f & rect) override;
		bool updateCompleted() const override;
        int32_t animate() override;

    public:
        core::aligns wheelFreedom() const override;
        void ondraw(drawing::Graphics & graphics, const core::rc32f & clip) const override;
        std::shared_ptr<Control> findChild(const core::pt32f & pos, std::shared_ptr<Control> last = nullptr, findchild_flags flags = nullptr) const override;

        void onPosChanged(const core::pt32f & from, const core::pt32f & to) override;
        void onSizeChanged(const core::si32f & from, const core::si32f & to) override;
        void onMouseWheel(const input_state & state) override;

    public:
        void relayout(layout_flags flags = nullptr);
        void setLayoutDirection(core::align layout);
        core::align layoutDirection() const { return _layout_direction; }
        void setCompactLayout(bool fall);
        void setScrollbarVisionV(scrollbar_vision scrollbar_vision);
        void setScrollbarVisionH(scrollbar_vision scrollbar_vision);

    protected:
        virtual void layout(layout_flags flags);
        virtual void onLayoutedSizeChaged(const core::si32f & from, const core::si32f & to);

    protected:
        void setLayoutedSize(const core::si32f & layouted_size);

    private:
        void onScrollValueChangedV(float32_t from, float32_t to);
        void onScrollValueChangedH(float32_t from, float32_t to);

    private:
        void _animScroll(core::vec2f scroll_pos);
        void _setScrollPos(core::vec2f scroll_pos);

    public:
        core::event<void(const core::si32f & from, const core::si32f & to)> layoutedSizeChaged;

    protected:
        std::vector<std::shared_ptr<Control>> _controls;
        core::align _layout_direction = core::align::left;
        bool _clip_clild = true;
        bool _compact_layout = true;

        core::si32f _layouted_size;

        core::vec2f _scroll_pos;
        scrollbar_vision _scrollbar_vision_v = scrollbar_vision::auto_hide;
        scrollbar_vision _scrollbar_vision_h = scrollbar_vision::auto_hide;
        std::shared_ptr<controls::ScrollBar> _scrollbar_v;
        std::shared_ptr<controls::ScrollBar> _scrollbar_h;
        bool _invalid_layout = false;
        layout_flags _invalid_layout_flags = nullptr;

        std::shared_ptr<core::property_animation> _scroll_anim;
    };
}
