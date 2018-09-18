#pragma once
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
        disabled,
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

        void enteringScene(std::shared_ptr<Scene> & scene) override;
        void enterScene(std::shared_ptr<Scene> & scene) override;
        void leavingScene() override;
        void leaveScene() override;

        core::si32f contentSize() const override;
        void layout(layout_flags flags);
        void arrange();
        void update() override;
        int32_t animate() override;

        void ondraw(drawing::Graphics & graphics, const drawing::Region & region) const override;
        std::shared_ptr<Control> findChild(const core::pt32f & pos, std::shared_ptr<Control> last = nullptr) const override;

        void onPosChanged(const core::pt32f & from, const core::pt32f & to) override;
        void onSizeChanged(const core::si32f & from, const core::si32f & to) override;

        void setScrollbarVisionV(scrollbar_vision scrollbar_vision);
        void setScrollbarVisionH(scrollbar_vision scrollbar_vision);

        void relayout();

    private:
        void onScrollValueChangedV(float32_t from, float32_t to);
        void onScrollValueChangedH(float32_t from, float32_t to);

    protected:
        virtual void onLayoutedSizeChaged(const core::si32f & from, const core::si32f & to);

    protected:
        void setLayoutedSize(const core::si32f & layouted_size);

    public:
        core::event<void(const core::si32f & from, const core::si32f & to)> layoutedSizeChaged;

    protected:
        std::multimap<int32_t, std::shared_ptr<Control>> _controls;
        core::align _layout_direction = core::align::left;
        bool _top_level = false;

        scrollbar_vision _scrollbar_vision_v = scrollbar_vision::auto_hide;
        scrollbar_vision _scrollbar_vision_h = scrollbar_vision::auto_hide;
        std::shared_ptr<controls::ScrollBar> _scrollbar_v;
        std::shared_ptr<controls::ScrollBar> _scrollbar_h;

        core::si32f _layouted_size;
        bool _invalid_layout = false;
        bool _clip_clild = true;
    };
}
