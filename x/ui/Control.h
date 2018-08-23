#pragma once
#include "component/Scene.h"
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

    class Control : public core::object
    {
    public:
        Control();
        virtual ~Control();

        std::shared_ptr<Control> ref() const { return const_cast<Control *>(this)->share_ref<Control>(); }
        static void propertyTableCallback(core::property_table & properties);
        virtual void propertyTable(core::property_table & properties);
        virtual const core::property_table & properties();

        void setStyleSheet(std::shared_ptr<component::StyleSheet> styleSheet);
        std::shared_ptr<component::StyleSheet> styleSheet() const;

        void setLayoutOrigin(layout_origin origin) { _layout_origin = origin; }
        layout_origin layoutOrigin() const { return _layout_origin; }

        void setPos(const core::vec2<core::dimensionf> & pos) { _pos = pos; }
        const core::vec2<core::dimensionf> & pos() const { return _pos; }
        void setSize(const core::vec2<core::dimensionf> & size) { _size = size; }
        const core::vec2<core::dimensionf> & size() const { return _size; }
        void setMinSize(const core::vec2<core::dimensionf> & minSize) { _minSize = minSize; }
        const core::vec2<core::dimensionf> & minSize() const { return _minSize; }
        void setMaxSize(const core::vec2<core::dimensionf> & maxSize) { _maxSize = maxSize; }
        const core::vec2<core::dimensionf> & maxSize() const { return _maxSize; }

        void move(const core::vec2<core::dimensionf> & pos);
        void resize(const core::vec2<core::dimensionf> & size);

        void setShowPos(const core::vec2f & pos);
        void setShowSize(const core::vec2f & size);
        void setShowRect(const core::rc32f & size);

        // expectSize 是一个不依赖父控件大小的『期望大小』，由控件本身决定
        core::si32f expectSize() const { return prefferSize(calc_flag::donot_calc_percent_xy); }
        // prefferSize 计算出的期望大小
        core::si32f prefferSize(calc_flag flags = calc_flag::none) const;
        virtual core::si32f contentSize() const { return core::si32f(); }

        std::shared_ptr<component::Animation> animation() const;
        std::shared_ptr<component::Scene> scene() const { return _scene.lock(); }
        void setParent(std::shared_ptr<Control> parent) { _parent = parent; }
        std::shared_ptr<Control> parent() const { return _parent.lock(); }

        const core::color32 & color() const;
        const graphics::font & font() const;

        void setVisible(bool vis) { if (_visible != vis) { _visible = vis; onVisibleChanged(vis); } }
        bool visible() const { return _visible.value; }

        core::si32f realPos() const { return _rect.pos; }
        core::si32f realSize() const { return _rect.size; }
        core::rc32f realRect() const { return _rect; }
        float32_t width() const { return _rect.cx; }
        float32_t height() const { return _rect.cy; }
        core::vec4f realMargin() const { return calc(_margin); }

        float32_t calc_x(const core::dimensionf & value, calc_flag flags = calc_flag::none) const;
        float32_t calc_y(const core::dimensionf & value, calc_flag flags = calc_flag::none) const;
        core::vec2f calc(const core::vec2<core::dimensionf> & value, calc_flag flags = calc_flag::none) const;
        core::vec4f calc(const core::vec4<core::dimensionf> & value, calc_flag flags = calc_flag::none) const;

       core::rc32f box() const;
       core::rc32f controlBox() const { return box(); }
       core::rc32f borderBox() const;
       core::rc32f paddingBox() const;
       core::rc32f contentBox() const;
       core::rc32f box(control_box box) const;

        void invalid();
        void relayout();
        void rearrange();

        void setBackgroundColor(core::color32 color);
        core::color32 backgroundColor() const;

        void setBackgroundImage(std::shared_ptr<graphics::Image> image);
        std::shared_ptr<graphics::Image> backgroundImage() const;

        void setMargin(const core::vec4<core::dimensionf> & margin) { if (_margin != margin) { _margin = margin; rearrange(); } }
        const core::vec4<core::dimensionf> & margin() const { return _margin; }

        void setBorder(const core::vec4<core::dimensionf> & border) { _border = border; }
        const core::vec4<core::dimensionf> & border() const { return _border; }

        void setPadding(const core::vec4<core::dimensionf> & padding) { if (_padding != padding) { _padding = padding; rearrange(); } }
        const core::vec4<core::dimensionf> & padding() const { return _padding; }

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

        // rect 控件应该定位的范围
        // size 控件的预计尺寸
        virtual void layout();
        virtual void arrange(const core::rc32f & rect, const core::si32f & size);
        virtual std::string styleName() const { return {}; }

        virtual void updateStyle();
        virtual void update();
        virtual void updateContent() {}
        virtual void onPosChanged(const core::pt32f & from, const core::pt32f & to);
        virtual void onSizeChanged(const core::si32f & from, const core::si32f & to);
        virtual void onRectChanged(const core::rc32f & from, const core::rc32f & to);
        virtual void onVisibleChanged(bool vis);

    public:
        core::event<void(const core::pt32f & from, const core::pt32f & to)> posChanged;
        core::event<void(const core::si32f & from, const core::si32f & to)> sizeChanged;
        core::event<void(const core::rc32f & from, const core::rc32f & to)> rectChanged;
        core::event<void(bool vis)> visibleChanged;

    protected:
        void _updateBackground();
        void _updateBorder();
        void _adjustSizeMinMax(core::si32f & size) const;

    protected:
        std::weak_ptr<component::Scene> _scene;
        std::weak_ptr<Control> _parent;

        std::shared_ptr<component::Animation> _animation;

        layout_origin _layout_origin = layout_origin::layout;

        core::attribute<core::color32> _color;
        // 控件大小，包括 padding，不包括 margin。
        core::attribute<core::vec2<core::dimensionf>> _pos;
        core::attribute<core::vec2<core::dimensionf>> _size;
        core::attribute<core::vec4<core::dimensionf>> _padding;
        core::attribute<core::vec4<core::dimensionf>> _border;
        core::attribute<core::vec4<core::color32>> _border_colors;
        core::attribute<core::vec4<graphics::stroke_style>> _border_styles;
        core::attribute<core::vec4<core::dimensionf>> _margin;
        core::attribute<core::vec2<core::dimensionf>> _minSize;
        core::attribute<core::vec2<core::dimensionf>> _maxSize;

        core::attribute<graphics::font> _font;

        core::attribute<bool> _visible = true;
        // background
        core::attribute<std::shared_ptr<graphics::Image>> _background_image;
        core::attribute<core::color32> _background_color = core::colors::Auto;
        layout_origin _background_position;
        core::attribute<core::vec2<core::dimensionf>> _background_size;
        core::vec2<renderables::image_fitting> _background_fitting = core::vec2<renderables::image_fitting>(renderables::image_fitting::none, renderables::image_fitting::none);
        control_box _background_box = control_box::layout_box;

        std::shared_ptr<component::StyleSheet> _styleSheet;
        std::string _style;
        bool _styleTransition = true;
        // 布局之后
        core::pt32f _location;
        core::rc32f _rect;

        std::shared_ptr<renderables::Image> _background_imgage_obj;
        std::shared_ptr<renderables::Rectangle> _background_rect_obj;
        std::shared_ptr<renderables::Rectangle> _border_obj;
        std::array<std::shared_ptr<renderables::Line>, 4> _border_objs;

        // true if need update
        bool _invalid = false;
        bool _invalid_layout = false;

        core::float3x2 _transform;
        std::multimap<int32_t, std::shared_ptr<component::Renderable>> _renderables;
        std::list<std::shared_ptr<component::MouseArea>> _mouseareas;

        core::rc32f _rect_invalid;

        std::mutex _mtx;
   public:
       void invalid_rect(const core::rc32f & rect);

       void clear();
       void insert(std::shared_ptr<component::Component> object) { insert(DEPTH_CONTENT, object); }
       void insert(int32_t depth, std::shared_ptr<component::Component> object);
       void remove(std::shared_ptr<component::Component> object);

       virtual void render(graphics::Graphics & graphics, const graphics::Region & region) const;

       std::shared_ptr<component::MouseArea> findMouseArea(const core::pt32f & pos, std::shared_ptr<component::MouseArea> last = nullptr) const;
    };
}
