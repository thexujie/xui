#pragma once
#include "component/Scene.h"
#include "attribute.h"
#include "View.h"

namespace controls
{
    class LayoutState
    {
        enum class step
        {
            size = 0,
            pos,
        };
    public:
        step step()
        {
            return step::size;
        }
        core::pt32f currPos()
        {
            return {};
        }
    };

    class Control
    {
    public:
        Control();
        virtual ~Control();

        void setRect(const core::rc32f & rect);
        core::rc32f rect() const { return _view_rect; }
        void setPos(const core::pt32f & pos);
        core::pt32f pos() const { return _view_rect.pos; }
        void setSize(const core::si32f & size);
        core::si32f size() const { return _view_rect.size; }
        float32_t width() const { return _view_rect.cx; }
        float32_t height() const { return _view_rect.cy; }

        void setMinSize(const unit_value<core::si32f> & minSize);
        unit_value<core::si32f> minSize() const { return _minSize; }
        void setMaxSize(const unit_value<core::si32f> & minSize);
        unit_value<core::si32f> maxSize() const { return _maxSize; }
        core::si32f prefferdSize() const;
        virtual core::si32f contentSize() const { return {}; }

        std::shared_ptr<View> view() const;
        std::shared_ptr<component::Scene> scene() const { return _scene.lock(); }
        std::shared_ptr<Control> parent() const { return _parent.lock(); }

        const core::color32 & color() const;
        const graphics::font & font() const;

        float32_t map(const unit_value<float32_t> & value, float32_t ref) const;
        core::vec2<float32_t> map(const core::vec2<unit_value<float32_t>> & size) const;
        core::vec4<float32_t> map(const core::vec4<unit_value<float32_t>> & rect) const;

       core::rc32f borderBox() const;
       core::rc32f paddingBox() const;
       core::rc32f contentBox() const;
       core::rc32f box(control_box box) const;

        void invalid();

        void setBackgroundColor(core::color32 color);
        core::color32 backgroundColor() const;
        void setBackgroundImage(std::shared_ptr<graphics::Image> image);
        std::shared_ptr<graphics::Image> backgroundImage() const;

        virtual void enteringScene(std::shared_ptr<component::Scene> & scene);
        virtual void enterScene(std::shared_ptr<component::Scene> & scene);
        virtual void leavingScene(std::shared_ptr<component::Scene> & scene);
        virtual void leaveScene(std::shared_ptr<component::Scene> & scene);

        virtual void layoutContent();
        virtual void layout(const core::pt32f & pos);
        virtual void update();
        virtual void onRectChanged(const core::rc32f & from, const core::rc32f & to);

    public:
        core::event<void(const core::rc32f & from, const core::rc32f & to)> rectChanged;

    private:
        void _updateBackground() const;

    protected:
        std::weak_ptr<component::Scene> _scene;
        std::weak_ptr<Control> _parent;
        std::shared_ptr<View> _view;

        position_origin _layout_origin = position_origin::layout;
        attribute<core::vec2<unit_value<float32_t>>> _position;

        attribute<core::color32> _color;
        // 控件大小，包括 padding，不包括 margin。
        attribute<core::vec2<unit_value<float32_t>>> _size;
        attribute<core::vec4<unit_value<float32_t>>> _margin;
        attribute<core::vec4<unit_value<float32_t>>> _paddding;

        attribute<graphics::font> _font;
        unit_value<core::si32f> _minSize;
        unit_value<core::si32f> _maxSize;

        // border
        // border 不接受 百分比 单位
        conntrol_border _border_left;
        conntrol_border _border_top;
        conntrol_border _border_right;
        conntrol_border _border_bottom;

        // background
        core::color32 _background_color = core::colors::Auto;
        std::shared_ptr<graphics::Image> _background_image;
        position_origin _background_position;
        attribute<core::vec2<unit_value<float32_t>>> _background_size;
        image_repeat _background_repeat = image_repeat::repeat;
        control_box _background_origin = control_box::padding_box;

        // 布局之后
        core::rc32f _view_rect;
        core::vec4<float32_t> _view_border;
    };
}
