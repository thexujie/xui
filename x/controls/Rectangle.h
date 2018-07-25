#pragma once
#include "renderables/Rectangle.h"
#include "View.h"

namespace controls
{
    class Rectangle : public View
    {
    public:
        Rectangle();
        explicit Rectangle(const core::rc32f & rect);
        ~Rectangle();

        void setPathStyle(const graphics::PathStyle & style);

    public:
        void enteringScene(std::shared_ptr<component::Scene> & scene) override;
        void enterScene(std::shared_ptr<component::Scene> & scene) override;
        void leaveScene(std::shared_ptr<component::Scene> & scene) override;
        void onRectChanged(const core::rc32f & from, const core::rc32f & to) override;

    private:
        graphics::PathStyle _style;
        std::shared_ptr<renderables::Rectangle> _rectangle;
    };
}
