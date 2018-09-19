#pragma once
#include "Container.h"
#include "controls/ScrollBar.h"

namespace ui
{
    class Grid : public Container
    {
    public:
        Grid();
        virtual ~Grid();

        void setLayoutDirection(core::align layout);

        core::si32f contentSize() const override;
        void layout(layout_flags flags);

        void onPosChanged(const core::pt32f & from, const core::pt32f & to) override;
        void onSizeChanged(const core::si32f & from, const core::si32f & to) override;

        void relayout();

    private:
        void onScrollValueChangedV(float32_t from, float32_t to);
        void onScrollValueChangedH(float32_t from, float32_t to);

    protected:
        core::align _layout_direction = core::align::left;
        bool _invalid_layout = false;
    };
}
