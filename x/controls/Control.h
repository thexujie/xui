#pragma once
#include "component/Scene.h"
#include "attribute.h"
#include "View.h"

namespace controls
{
    enum class layout_step
    {
        // 根据父控件大小来计算自己的大小
        parent = 0,
        // 根据最佳尺寸来计算自己的大小
        preffer,
        // 
        layout
    };

    class LayoutState
    {
    public:
        bool finish() const { return !_numInvalid; }


    private:
        layout_step _step = layout_step::parent;
        int32_t _numInvalid = 0;
    };

    class Control : public std::enable_shared_from_this<Control>
    {
    public:
        Control();
        virtual ~Control();

        void setRect(const core::vec4<core::dimensionf> & rect) { _pos = rect.pos; _size = rect.size; }
        core::vec4<core::dimensionf> rect() const { return {_pos, _size}; }
        void setPos(const core::vec2<core::dimensionf> & pos) { _pos = pos; }
        const core::vec2<core::dimensionf> & pos() const { return _pos; }
        void setSize(const core::vec2<core::dimensionf> & size) { _size = size; }
        const core::vec2<core::dimensionf> & size() const { return _size; }
        void setMinSize(const core::unit_value<core::si32f> & minSize);
        const core::vec2<core::dimensionf> & minSize() const { return _minSize; }
        void setMaxSize(const core::unit_value<core::si32f> & minSize);
        const core::vec2<core::dimensionf> & maxSize() const { return _maxSize; }

        // expectSize 是一个不依赖父控件大小的『期望大小』，由控件本身决定
        core::si32f expectSize() const;
        virtual core::si32f expectContentSize() const { return core::si32f(); }
        std::shared_ptr<View> view() const;
        std::shared_ptr<component::Scene> scene() const { return _scene.lock(); }
        void setParent(std::shared_ptr<Control> parent) { _parent = parent; }
        std::shared_ptr<Control> parent() const { return _parent.lock(); }

        const core::color32 & color() const;
        const graphics::font & font() const;

        core::si32f dimension() const { return _view_rect.size; }
        float32_t width() const { return _view_rect.cx; }
        float32_t height() const { return _view_rect.cy; }
        core::si32f contentSize() const { return _view_content_size; }
        core::vec4f border() const { return _view_border; }
        core::vec4f margin() const { return _view_margin; }

        float32_t calc(const core::dimensionf & value) const;
        core::vec2f calc(const core::vec2<core::dimensionf> & value) const;
        core::vec4f calc(const core::vec4<core::dimensionf> & value) const;

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

        virtual void layout(LayoutState & state, const core::rc32f & rect);

        virtual void update();
        virtual void onRectChanged(const core::rc32f & from, const core::rc32f & to);

    public:
        core::event<void(const core::rc32f & from, const core::rc32f & to)> rectChanged;

    protected:
        void _updateBackground() const;
        void _adjustSizeMinMax(core::si32f & size) const;

    protected:
        std::weak_ptr<component::Scene> _scene;
        std::weak_ptr<Control> _parent;
        std::shared_ptr<View> _view;

        position_origin _layout_origin = position_origin::layout;
        attribute<core::vec2<core::dimensionf>> _position;

        attribute<core::color32> _color;
        // 控件大小，包括 padding，不包括 margin。
        attribute<core::vec2<core::dimensionf>> _pos;
        attribute<core::vec2<core::dimensionf>> _size;
        attribute<core::vec4<core::dimensionf>> _paddding;
        attribute<core::vec4<core::dimensionf>> _border_inner;
        attribute<core::vec4<core::dimensionf>> _border_outter;
        attribute<core::vec4<core::dimensionf>> _margin;
        attribute<core::vec2<core::dimensionf>> _minSize;
        attribute<core::vec2<core::dimensionf>> _maxSize;

        attribute<graphics::font> _font;

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
        attribute<core::vec2<core::dimensionf>> _background_size;
        image_repeat _background_repeat = image_repeat::repeat;
        control_box _background_box = control_box::padding_box;

        // 布局之后
        core::rc32f _view_rect;
        core::vec4f _view_border;
        core::vec4f _view_padding;
        core::vec4f _view_margin;
        core::si32f _view_content_size;
    };
}
