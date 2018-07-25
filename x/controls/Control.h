#pragma once
#include "component/Scene.h"
#include "attribute.h"
#include "View.h"

namespace controls
{
    class Control
    {
    public:
        Control();
        virtual ~Control();

        void setRect(const core::rc32f & rect);
        core::rc32f rect() const { return _rect; }
        void setPos(const core::pt32f & pos);
        core::pt32f pos() const { return _rect.position; }
        void setSize(const core::si32f & size);
        core::si32f size() const { return _rect.size; }

        core::si32f map(const unit_value<core::si32f> & value) const;

        const core::rc32f & borderBox() const;
        const core::rc32f & paddingBox() const;
        const core::rc32f & contentBox() const;
        const core::rc32f & box(control_box box) const;

        void invalid();

        void setBackgroundColor(core::color32 color);
        core::color32 backgroundColor() const;
        void setBackgroundImage(std::shared_ptr<graphics::Image> image);
        std::shared_ptr<graphics::Image> backgroundImage() const;

        std::shared_ptr<component::Scene> scene() const;
        virtual void enteringScene(std::shared_ptr<component::Scene> & scene);
        virtual void enterScene(std::shared_ptr<component::Scene> & scene);
        virtual void leavingScene(std::shared_ptr<component::Scene> & scene);
        virtual void leaveScene(std::shared_ptr<component::Scene> & scene);

        virtual void onUpdate(graphics::Graphics & graphics);
        virtual void onRectChanged(const core::rc32f & from, const core::rc32f & to);

    public:
        core::event<void(const core::rc32f & from, const core::rc32f & to)> rectChanged;

    private:
        void _updateBackground() const;

    protected:
        std::weak_ptr<component::Scene> _scene;
        std::shared_ptr<View> _view;

        core::rc32f _rect;

        // border
        conntrol_border _border;
        conntrol_border _border_top;
        conntrol_border _border_right;
        conntrol_border _border_bottom;
        conntrol_border _border_left;

        // background
        core::color32 _background_color = core::colors::Auto;
        std::shared_ptr<graphics::Image> _background_image;
        layout_position _background_position;
        unit_value<core::si32f> _background_size;
        image_repeat _background_repeat = image_repeat::repeat;
        control_box _background_origin = control_box::padding_box;
    };
}
