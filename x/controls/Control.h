#pragma once
#include "component/Scene.h"
#include "attribute.h"
#include "View.h"

namespace controls
{
    class Control : public std::enable_shared_from_this<Control>
    {
    public:
        Control();
        virtual ~Control();

        void setLayoutOrigin(layout_origin origin) { _layout_origin = origin; }
        layout_origin layoutOrigin() const { return _layout_origin; }

        void setPos(const core::vec2<core::dimensionf> & pos);
        const core::vec2<core::dimensionf> & pos() const { return _pos; }
        void setSize(const core::vec2<core::dimensionf> & size) { _size = size; }
        const core::vec2<core::dimensionf> & size() const { return _size; }
        void setMinSize(const core::vec2<core::dimensionf> & minSize) { _minSize = minSize; }
        const core::vec2<core::dimensionf> & minSize() const { return _minSize; }
        void setMaxSize(const core::vec2<core::dimensionf> & maxSize) { _maxSize = maxSize; }
        const core::vec2<core::dimensionf> & maxSize() const { return _maxSize; }

        void setPos(const core::vec2f & pos);

        // prefferSize 是一个不依赖父控件大小的『期望大小』，由控件本身决定
        core::si32f prefferSize() const;
        virtual core::si32f contentSize() const { return core::si32f(); }

        std::shared_ptr<View> view() const;
        std::shared_ptr<component::Scene> scene() const { return _scene.lock(); }
        void setParent(std::shared_ptr<Control> parent) { _parent = parent; }
        std::shared_ptr<Control> parent() const { return _parent.lock(); }

        const core::color32 & color() const;
        const graphics::font & font() const;

        core::si32f realPos() const { return calc(_pos); }
        core::si32f realSize() const { return calc(_size); }
        float32_t width() const { return _rect.cx; }
        float32_t height() const { return _rect.cy; }
        core::vec4f border() const { return calc(_border); }
        core::vec4f margin() const { return calc(_margin); }


        float32_t calc(const core::dimensionf & value) const;
        core::vec2f calc(const core::vec2<core::dimensionf> & value) const;
        core::vec4f calc(const core::vec4<core::dimensionf> & value) const;

       core::rc32f box() const;
       core::rc32f borderBox() const;
       core::rc32f paddingBox() const;
       core::rc32f contentBox() const;
       core::rc32f box(control_box box) const;

        void invalid();

        void setBackgroundColor(core::color32 color);
        core::color32 backgroundColor() const;
        void setBackgroundImage(std::shared_ptr<graphics::Image> image);
        std::shared_ptr<graphics::Image> backgroundImage() const;
        void setMargin(const core::vec4<core::dimensionf> & margin) { _margin = margin; }
        void setBorder(const core::vec4<core::dimensionf> & border) { _border = border; }
        void setPadding(const core::vec4<core::dimensionf> & padding) { _padding = padding; }
        void setBorderColors(const core::vec4<core::color32> & boderColors) { _border_colors = boderColors; }
        void setBorderStyles(const core::vec4<graphics::stroke_style> & boderStyles) { _border_styles = boderStyles; }
        const core::vec4<core::color32> & boderColors() const { return _border_colors; }
        std::array<core::pt32f, 4> boderPoints(core::align edge) const;
        std::array<core::pt32f, 2> boderLine(core::align edge) const;

        virtual void enteringScene(std::shared_ptr<component::Scene> & scene);
        virtual void enterScene(std::shared_ptr<component::Scene> & scene);
        virtual void leavingScene(std::shared_ptr<component::Scene> & scene);
        virtual void leaveScene(std::shared_ptr<component::Scene> & scene);

        // rect 控件应该定位的范围
        // size 控件的预计尺寸
        virtual void layout() {}
        virtual void layout(const core::rc32f & rect, const core::si32f & size);

        virtual void update();
        virtual void updateContent() {}
        virtual void onRectChanged(const core::rc32f & from, const core::rc32f & to);

    public:
        core::event<void(const core::pt32f & from, const core::pt32f & to)> posChanged;
        core::event<void(const core::rc32f & from, const core::rc32f & to)> rectChanged;

    protected:
        void _updateBackground() const;
        void _updateBorder() const;
        void _adjustSizeMinMax(core::si32f & size) const;

    protected:
        std::weak_ptr<component::Scene> _scene;
        std::weak_ptr<Control> _parent;
        std::shared_ptr<View> _view;

        layout_origin _layout_origin = layout_origin::layout;

        attribute<core::color32> _color;
        // 控件大小，包括 padding，不包括 margin。
        attribute<core::vec2<core::dimensionf>> _pos;
        attribute<core::vec2<core::dimensionf>> _size;
        attribute<core::vec4<core::dimensionf>> _padding;
        attribute<core::vec4<core::dimensionf>> _border;
        attribute<core::vec4<core::color32>> _border_colors;
        attribute<core::vec4<graphics::stroke_style>> _border_styles;
        attribute<core::vec4<core::dimensionf>> _margin;
        attribute<core::vec2<core::dimensionf>> _minSize;
        attribute<core::vec2<core::dimensionf>> _maxSize;

        attribute<graphics::font> _font;

        // background
        core::color32 _background_color = core::colors::Auto;
        std::shared_ptr<graphics::Image> _background_image;
        layout_origin _background_position;
        attribute<core::vec2<core::dimensionf>> _background_size;
        core::vec2<image_fitting> _background_fitting = core::vec2<image_fitting>(image_fitting::none, image_fitting::none);
        control_box _background_box = control_box::border_box;

        // 布局之后
        core::rc32f _rect;
    };
}
