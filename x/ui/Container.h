#pragma once
#include "component/Scene.h"
#include "Control.h"

namespace ui
{
    namespace controls
    {
        class ScrollBar;
    }


    enum class scrollbar_vision
    {
        auto_hide = 0,
        always,
    };

    class Container : public Control
    {
    public:
        Container();
        virtual ~Container();

        void setLayoutDirection(core::align layout);

        void addControl(std::shared_ptr<Control> control);
        void removeControl(std::shared_ptr<Control> control);
        std::multimap<int32_t, std::shared_ptr<Control>> & children() { return _controls; }

        void enteringScene(std::shared_ptr<component::Scene> & scene) override;
        void enterScene(std::shared_ptr<component::Scene> & scene) override;
        void leavingScene() override;
        void leaveScene() override;

        core::si32f contentSize() const override;
        void layout(layout_flags flags) override;
        void update() override;

        void onSizeChanged(const core::si32f & from, const core::si32f & to) override;

        void setScrollbarVisionV(scrollbar_vision scrollbar_vision);
    protected:
        std::multimap<int32_t, std::shared_ptr<Control>> _controls;
        core::align _layout_direction = core::align::left;
        bool _top_level = false;

        scrollbar_vision _scrollbar_vision_v = scrollbar_vision::auto_hide;
        std::shared_ptr<controls::ScrollBar> _scrollbar_v;
        std::shared_ptr<controls::ScrollBar> _scrollbar_h;
    };
}
