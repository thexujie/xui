#pragma once
#include "Container.h"

namespace ui
{
    namespace controls
    {
        class ScrollBar;
    }

    enum class scrollbar_vision
    {
        auto_hide = 0,
        disabled,
        always,
    };

    class Scrollable : public Container
    {
    public:
        Scrollable();
        virtual ~Scrollable();

        void setLayoutDirection(core::align layout);

        core::si32f contentSize() const override;
        void layout(layout_flags flags);

        void onPosChanged(const core::pt32f & from, const core::pt32f & to) override;
        void onSizeChanged(const core::si32f & from, const core::si32f & to) override;

        void setScrollbarVisionV(scrollbar_vision scrollbar_vision);
        void setScrollbarVisionH(scrollbar_vision scrollbar_vision);

        void relayout();

    private:
        void onScrollValueChangedV(float32_t from, float32_t to);
        void onScrollValueChangedH(float32_t from, float32_t to);

    protected:
        core::align _layout_direction = core::align::left;

        scrollbar_vision _scrollbar_vision_v = scrollbar_vision::auto_hide;
        scrollbar_vision _scrollbar_vision_h = scrollbar_vision::auto_hide;
        std::shared_ptr<controls::ScrollBar> _scrollbar_v;
        std::shared_ptr<controls::ScrollBar> _scrollbar_h;
        bool _invalid_layout = false;
    };
}
