#pragma once
#include "component/Scene.h"
#include "component/View.h"
#include "renderables/Text.h"
#include "renderables/Image.h"
#include "renderables/Line.h"
#include "renderables/Rectangle.h"
#include "component/Style.h"
#include "component/Animation.h"

namespace ui
{
    const int32_t DEPTH_BACKGROUND = -1;
    const int32_t DEPTH_CONTENT = 0;
    const int32_t DEPTH_FOREGROUND = 1;

    enum class control_box
    {
        // 控件边框
        layout_box = 0,
        // 边框方框
        border_box,
        // 内框方框
        padding_box,
        // 内容方框
        content_box,
    };

    enum class layout_origin
    {
        // 默认位置
        layout = 0,
        // 相对于 parent 的位置
        parent,
        // 相对于 scene 的位置
        scene,
        // 相对于 view 的位置
        view,
        // 同 arrange，如果超出 scene 则相对于 view
        sticky,
    };

    enum class calc_flag
    {
        none = 0,
        donot_calc_percent_x = 0x0001,
        donot_calc_percent_y = 0x0002,
        donot_calc_percent_xy = donot_calc_percent_x | donot_calc_percent_y,
    };
    template<> struct enable_bitmasks<calc_flag> { static const bool enable = true; };

    class Element : public core::object
    {
    public:
        Element();
        virtual ~Element();

        void setLocation(const core::vec2f & pos);
        void setMeasure(const core::vec2f & size);
        void setRect(const core::rc32f & size);

        core::rc32f box() const;
        core::rc32f borderBox() const;
        core::rc32f paddingBox() const;
        core::rc32f contentBox() const;
        core::rc32f box(control_box box) const;

        std::shared_ptr<component::View> view() const;
        std::shared_ptr<component::Animation> animation() const;
        std::shared_ptr<component::Scene> scene() const { return _scene.lock(); }
        void setParent(std::shared_ptr<Element> parent) { _parent = parent; }
        std::shared_ptr<Element> parent() const { return _parent.lock(); }

        core::si32f realPos() const { return _rect.pos; }
        core::si32f realSize() const { return _rect.size; }
        core::rc32f realRect() const { return _rect; }
        float32_t width() const { return _rect.cx; }
        float32_t height() const { return _rect.cy; }

        void setBackgroundColor(core::color32 color);
        core::color32 backgroundColor() const;

        void setBackgroundImage(std::shared_ptr<graphics::Image> image);
        std::shared_ptr<graphics::Image> backgroundImage() const;

        void setBorder(const core::vec4<core::float32_t> & border) { _border = border; }
        const core::vec4<core::float32_t> & border() const { return _border; }

        void setBorderColors(const core::vec4<core::color32> & boderColors) { _border_colors = boderColors; }
        const core::vec4<core::color32> & borderColors() const { return _border_colors; }
        void setBorderStyles(const core::vec4<graphics::stroke_style> & boderStyles) { _border_styles = boderStyles; }
        const core::vec4<core::color32> & boderColors() const { return _border_colors; }
        std::array<core::pt32f, 4> boderPoints(core::align edge) const;
        std::array<core::pt32f, 2> boderLine(core::align edge) const;

        virtual void enteringScene(std::shared_ptr<component::Scene> & scene);
        virtual void enterScene(std::shared_ptr<component::Scene> & scene);
        virtual void leavingScene(std::shared_ptr<component::Scene> & scene);
        virtual void leaveScene(std::shared_ptr<component::Scene> & scene);

        virtual void updateContent(std::shared_ptr<component::View> & view) {}
        virtual void onLocationChanged(const core::pt32f & from, const core::pt32f & to);
        virtual void onMeasureChanged(const core::si32f & from, const core::si32f & to);
        virtual void onRectChanged(const core::rc32f & from, const core::rc32f & to);

    public:
        core::event<void(const core::pt32f & from, const core::pt32f & to)> locationChanged;
        core::event<void(const core::si32f & from, const core::si32f & to)> measureChanged;
        core::event<void(const core::rc32f & from, const core::rc32f & to)> rectChanged;

    protected:
        void _updateBackground(std::shared_ptr<component::View> & view);
        void _updateBorder(std::shared_ptr<component::View> & view);
        void _adjustSizeMinMax(core::si32f & size) const;

    protected:
        std::weak_ptr<component::Scene> _scene;
        std::weak_ptr<Element> _parent;

        std::shared_ptr<component::View> _view;
        std::shared_ptr<component::Animation> _animation;

        layout_origin _layout_origin = layout_origin::layout;

        // 控件大小，包括 padding，不包括 margin。
        core::vec2<core::float32_t> _pos;
        core::vec2<core::float32_t> _size;
        core::vec4<core::float32_t> _padding;
        core::vec4<core::float32_t> _border;
        core::vec4<core::color32> _border_colors;
        core::vec4<graphics::stroke_style> _border_styles;
        core::vec4<core::float32_t> _margin;
        core::vec2<core::float32_t> _minSize;
        core::vec2<core::float32_t> _maxSize;

        core::attribute<graphics::font> _font;

        // background
        std::shared_ptr<graphics::Image> _background_image;
        core::color32 _background_color = core::colors::Auto;
        layout_origin _background_position;
        core::vec2<core::float32_t> _background_size;
        core::vec2<renderables::image_fitting> _background_fitting = core::vec2<renderables::image_fitting>(renderables::image_fitting::none, renderables::image_fitting::none);
        control_box _background_box = control_box::layout_box;

        std::shared_ptr<component::StyleSheet> _styleSheet;
        std::string _style;
        bool _styleTransition = true;
        // 布局之后
        core::rc32f _rect;

        std::shared_ptr<renderables::Image> _background_imgage_obj;
        std::shared_ptr<renderables::Rectangle> _background_rect_obj;
        std::shared_ptr<renderables::Rectangle> _border_obj;
        std::array<std::shared_ptr<renderables::Line>, 4> _border_objs;

        // true if need update
        bool _invalid = false;
        bool _invalid_layout = false;
    };
}
