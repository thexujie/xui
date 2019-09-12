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

    struct ControlSorter
    {
    public:
        bool operator()(const std::shared_ptr<Control> & lhs, const std::shared_ptr<Control> & rhs) const
        {
            return lhs->ZValue() < rhs->ZValue();
        }
    };

    class Container : public Control
    {
    public:
        Container();
        Container(core::align layout);
        virtual ~Container();

		void addControl(std::shared_ptr<Control> control);
        std::shared_ptr<controls::Spacer> addSpacer(core::dimenf size);
        void removeControl(std::shared_ptr<Control> control);
		void clearControls();
        std::multiset<std::shared_ptr<Control>, ControlSorter> & children() { return _controls; }

        void onEnteringScene(std::shared_ptr<Scene> & scene) override;
        void onEnterScene() override;
        void onLeavingScene() override;
        void onLeaveScene() override;

        //core::sizef contentSize() const override;
		bool validCompleted() const override;
        size_t onAnimate() override;

    public:
        core::aligns wheelFreedom() const override;
        void onPaint(drawing::Graphics & graphics, const core::rectf & clip) const override;
        std::shared_ptr<Control> findChild(const core::pointf & pos, std::shared_ptr<Control> last = nullptr, findchild_flags flags = nullptr) const override;

        void onPosChanged(const core::pointf & from, const core::pointf & to) override;
        void onSizeChanged(const core::sizef & from, const core::sizef & to) override;
        void onWheel(const input_state & state) override;

		void notifyShown(bool shown) override;
    public:
        void relayout();
        void setLayoutDirection(core::align layout);
        core::align layoutDirection() const { return _layout_direction; }
        void setScrollbarVisionV(scrollbar_vision scrollbar_vision);
        void setScrollbarVisionH(scrollbar_vision scrollbar_vision);

    protected:
        virtual void layout();
        virtual void onLayoutedSizeChaged(const core::sizef & from, const core::sizef & to);
        virtual void onScrollPosChanged(const core::vec2f & from, const core::vec2f & to);

    protected:
        void setScrollSize(const core::sizef & size);

    private:
        void onScrollBarValueChangedV(float32_t from, float32_t to);
        void onScrollBarValueChangedH(float32_t from, float32_t to);

	private:
        void _animScroll(core::vec2f scroll_pos);
        void _setScrollPos(core::vec2f scroll_pos);

    public:
        core::event<void(const core::sizef & from, const core::sizef & to)> layoutedSizeChaged;

    protected:
        //std::vector<std::shared_ptr<Control>> _controls;
        std::multiset<std::shared_ptr<Control>, ControlSorter> _controls;
        core::align _layout_direction = core::align::left;
        bool _clip_clild = true;
        bool _compact_layout = true;
        bool _scroll_controls = true;
        bool _control_content = true;

        core::sizef _scroll_size;
        core::vec2f _scroll_pos;
        scrollbar_vision _scrollbar_vision_v = scrollbar_vision::auto_hide;
        scrollbar_vision _scrollbar_vision_h = scrollbar_vision::auto_hide;
        std::shared_ptr<controls::ScrollBar> _scrollbar_v;
        std::shared_ptr<controls::ScrollBar> _scrollbar_h;

        std::shared_ptr<core::property_animation> _scroll_anim;
    };
}
