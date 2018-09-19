#pragma once
#include "Control.h"

namespace ui
{
    class Container : public Control
    {
    protected:
        Container();
        virtual ~Container();

    public:
        void addControl(std::shared_ptr<Control> control);
        void removeControl(std::shared_ptr<Control> control);
        std::multimap<int32_t, std::shared_ptr<Control>> & children() { return _controls; }

        void enteringScene(std::shared_ptr<Scene> & scene) override;
        void enterScene(std::shared_ptr<Scene> & scene) override;
        void leavingScene() override;
        void leaveScene() override;

        void update() override;
        int32_t animate() override;

        void ondraw(drawing::Graphics & graphics, const drawing::Region & region) const override;
        std::shared_ptr<Control> findChild(const core::pt32f & pos, std::shared_ptr<Control> last = nullptr) const override;

    public:
        virtual void relayout() {}

    protected:
        virtual void onLayoutedSizeChaged(const core::si32f & from, const core::si32f & to);

    protected:
        void setLayoutedSize(const core::si32f & layouted_size);

    public:
        core::event<void(const core::si32f & from, const core::si32f & to)> layoutedSizeChaged;

    protected:
        std::multimap<int32_t, std::shared_ptr<Control>> _controls;
        bool _clip_clild = true;

        core::si32f _layouted_size;
    };
}
