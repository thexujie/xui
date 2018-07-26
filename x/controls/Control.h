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

    class Control : public std::enable_shared_from_this<Control>
    {
    public:
        Control();
        virtual ~Control();

        void setRect(const core::vec4<core::unit_value<float32_t>> & rect) { _pos = rect.pos; _size = rect.size; }
        core::vec4<core::unit_value<float32_t>> rect() const { return {_pos, _size}; }
        void setPos(const core::vec2<core::unit_value<float32_t>> & pos) { _pos = pos; }
        const core::vec2<core::unit_value<float32_t>> & pos() const { return _pos; }
        void setSize(const core::vec2<core::unit_value<float32_t>> & size) { _size = size; }
        const core::vec2<core::unit_value<float32_t>> & size() const { return _size; }
        void setMinSize(const core::unit_value<core::si32f> & minSize);
        core::unit_value<core::si32f> minSize() const { return _minSize; }
        void setMaxSize(const core::unit_value<core::si32f> & minSize);
        core::unit_value<core::si32f> maxSize() const { return _maxSize; }

        std::shared_ptr<View> view() const;
        std::shared_ptr<component::Scene> scene() const { return _scene.lock(); }
        void setParent(std::shared_ptr<Control> parent) { _parent = parent; }
        std::shared_ptr<Control> parent() const { return _parent.lock(); }

        const core::color32 & color() const;
        const graphics::font & font() const;

        core::si32f dimension() const { return _view_rect.size; }
        float32_t width() const { return _view_rect.cx; }
        float32_t height() const { return _view_rect.cy; }
        virtual core::si32f contentSize() const { return {}; }
        core::vec4f border() const { return _view_border; }
        core::vec4f margin() const { return _view_margin; }

        float32_t calc(const core::unit_value<float32_t> & value) const;
        core::vec2f calc(const core::vec2<core::unit_value<float32_t>> & value) const;
        core::vec4f calc(const core::vec4<core::unit_value<float32_t>> & value) const;

        float32_t map(const core::unit_value<float32_t> & value, float32_t ref) const;
        core::vec2<float32_t> map(const core::vec2<core::unit_value<float32_t>> & size) const;
        core::vec4<float32_t> map(const core::vec4<core::unit_value<float32_t>> & rect) const;

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

        virtual void layout(const core::rc32f & rect);

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
        attribute<core::vec2<core::unit_value<float32_t>>> _position;

        attribute<core::color32> _color;
        // 控件大小，包括 padding，不包括 margin。
        attribute<core::vec2<core::unit_value<float32_t>>> _pos;
        attribute<core::vec2<core::unit_value<float32_t>>> _size;
        attribute<core::vec4<core::unit_value<float32_t>>> _paddding;
        attribute<core::vec4<core::unit_value<float32_t>>> _margin;

        attribute<graphics::font> _font;
        core::unit_value<core::si32f> _minSize;
        core::unit_value<core::si32f> _maxSize;

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
        attribute<core::vec2<core::unit_value<float32_t>>> _background_size;
        image_repeat _background_repeat = image_repeat::repeat;
        control_box _background_origin = control_box::padding_box;

        // 布局之后
        core::rc32f _view_rect;
        core::vec4f _view_border;
        core::vec4f _view_padding;
        core::vec4f _view_margin;
    };
}
