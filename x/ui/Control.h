#pragma once
#include "component/Style.h"
#include "renderables/Text.h"
#include "renderables/Image.h"
#include "renderables/Line.h"
#include "renderables/Rectangle.h"
#include "graphics/Region.h"
#include "Scene.h"

namespace ui
{
    class Scene;

    const int32_t ZVALUE_BACKGROUND = -100;
    const int32_t ZVALUE_CONTENT = 0;
    const int32_t ZVALUE_FOREGROUND = 100;
    const int32_t ZVALUE_TOPMOST = 0xFFFFFFF;
    const int32_t ZVALUE_MAX = 0xFFFFFFF;

    const int32_t ZVALUE_SCROLLBAR = 0x10000001;

    const int32_t LOCAL_DEPTH_BACKGROUND = -100;
    const int32_t LOCAL_DEPTH_CONTENT = 0;
    const int32_t LOCAL_DEPTH_FOREGROUND = 100;

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

    enum class hittest_result
    {
        nowhere = 0,
        // 这个位置可交互
        client,
        // 这个位置不可交互
        stable,
        // 鼠标可以从这个位置穿透过去
        transparent,
    };

    enum class layout_origin
    {
        // 默认位置
        layout = 0,
        // 相对于 parent 的位置
        parent,
        // 相对于 scene 的位置
        scene,
        // 同 layout，如果超出 scene 则按照 parent 处理
        sticky,
    };

    enum class layout_flag
    {
        none = 0,
        resize_cx = 0x0001,
        resize_cy = 0x0002,
        force = 0x0004,
    };
    template<> struct enable_bitmasks<layout_flag> { static const bool enable = true; };
    typedef core::bitflag<layout_flag> layout_flags;

    enum class calc_flag
    {
        none = 0,
        donot_calc_percent_x = 0x0001,
        donot_calc_percent_y = 0x0002,
        donot_calc_percent_xy = donot_calc_percent_x | donot_calc_percent_y,
    };
    template<> struct enable_bitmasks<calc_flag> { static const bool enable = true; };
    typedef core::bitflag<calc_flag> calc_flags;

    enum class mouse_button
    {
        none = 0,
        left = 0x0001,
        middle = 0x0002,
        right = 0x0004,
        mask = 0x00ff,
    };

    template<> struct enable_bitmasks<mouse_button> { static const bool enable = true; };
    typedef core::bitflag<mouse_button> mouse_buttons;

    enum class mouse_action
    {
        none = 0,
        enter,
        move,
        press,
        click,
        releas,
        dbclick,
        leave,

        wheel_v,
    };

    class mosue_state
    {
    public:
        mosue_state() = default;
        mosue_state(const core::pt32f pos) : _pos(pos) {}

        void setPos(const core::pt32f & pos) { _pos = pos; }
        const core::pt32f & pos() const { return _pos; }

        void setButton(mouse_button button, bool active) { active ? (_buttons |= button) : (_buttons &= ~button); }
        mouse_button buttons() const { return _buttons; }

        void setWheelLines(int32_t lines) { _wheel_lines = lines; }
        int32_t wheelLines() const { return _wheel_lines; }

    private:
        core::pt32f _pos;
        mouse_button _buttons = mouse_button::none;
        int32_t _wheel_lines = 0;
    };

    enum class ime_mode
    {
        disabled = 0,
        on,
        off,
    };

    class ImeContext
    {
    public:
        virtual ~ImeContext() {}
        virtual void setImeMode(ui::ime_mode mode) = 0;
        virtual void setCompositionPos(core::pt32f pos) = 0;
        virtual void setCompositionFont(const graphics::font & font) = 0;
    };

    class Control : public core::object
    {
    public:
        Control();
        virtual ~Control();

        std::shared_ptr<Control> control_ref() const { return const_cast<Control *>(this)->share_ref<Control>(); }
        static void propertyTableCallback(core::property_table & properties);
        virtual void propertyTable(core::property_table & properties);
        virtual const core::property_table & properties();

        void lock() const { _mtx.lock(); }
        void unlock() const { _mtx.unlock(); }

        void setStyleSheet(std::shared_ptr<component::StyleSheet> styleSheet);
        std::shared_ptr<component::StyleSheet> styleSheet() const;

        void setZValue(int32_t zvalue) { _zvalue = zvalue; }
        int32_t ZValue() const { return _zvalue; }

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

        void setAnchorBorders(core::aligns borders) { _anchor_borders = borders; }
        core::aligns anchorBorders() const { return _anchor_borders; }
        void setAnchor(const core::vec4<core::dimensionf> & anchor) { _anchor = anchor; }
        const core::vec4<core::dimensionf> & anchor() const { return _anchor; }

        void move(const core::vec2<core::dimensionf> & pos);
        void resize(const core::vec2<core::dimensionf> & size);

        void setShowPos(const core::vec2f & pos);
        void setShowSize(const core::vec2f & size);
        void setShowRect(const core::rc32f & size);

        // expectSize 是一个不依赖父控件大小的『期望大小』，由控件本身决定
        core::si32f expectSize() const { return prefferSize(calc_flag::donot_calc_percent_xy); }
        // prefferSize 计算出的期望大小
        core::si32f prefferSize(calc_flags flags = calc_flag::none) const;
        virtual core::si32f contentSize() const { return core::si32f(); }

        std::shared_ptr<Scene> scene() const { return _scene.lock(); }
        void setParent(std::shared_ptr<Control> parent) { _parent = parent; }
        std::shared_ptr<Control> parent() const { return _parent.lock(); }

        const core::color32 & color() const;
        const graphics::font & font() const;

        void setVisible(bool vis) { if (_visible != vis) { _visible = vis; onVisibleChanged(vis); } }
        bool visible() const { return _visible.value; }

        core::si32f realPos() const { return _rect.pos; }
        core::si32f realSize() const { return _rect.size; }
        float32_t realWidth() const { return _rect.cx; }
        float32_t realHeight() const { return _rect.cy; }
        core::rc32f realRect() const { return _rect; }
        float32_t width() const { return _rect.cx; }
        float32_t height() const { return _rect.cy; }
        core::vec4f realMargin() const { return calc(_margin); }

        float32_t calc_x(const core::dimensionf & value, calc_flags flags = calc_flag::none) const
        {
            auto s = scene();
            auto p = parent();
            if (!s)
                throw core::error_state;

            switch (value.unit)
            {
            case core::unit::px:
                return value.value * s->ratio();
            case core::unit::em:
                return value.value * graphics::fontmetrics(font()).height * s->ratio();
            case core::unit::pt:
                return value.value * 72.0f * s->ratio();
            case core::unit::dot:
                return value.value;
            case core::unit::per:
                if (flags.any(calc_flag::donot_calc_percent_x))
                    return 0;
                if (!p)
                    throw core::error_state;
                if(_margin.value.bleft.unit == core::unit::per || _margin.value.bright.unit == core::unit::per)
                    throw core::error_bad_data;
                return value.value / 100.0f * (p->width() - calc_x(_margin.value.bleft) - calc_x(_margin.value.bright));
            default:
                return value.value * s->ratio();
            }
        }
        float32_t calc_y(const core::dimensionf & value, calc_flags flags = calc_flag::none) const
        {
            auto s = scene();
            auto p = parent();
            if (!s)
                throw core::error_state;

            switch (value.unit)
            {
            case core::unit::px:
                return value.value * s->ratio();
            case core::unit::em:
                return value.value * graphics::fontmetrics(font()).height * s->ratio();
            case core::unit::pt:
                return value.value * 72.0f * s->ratio();
            case core::unit::dot:
                return value.value;
            case core::unit::per:
                if (flags.any(calc_flag::donot_calc_percent_y))
                    return 0;
                if (!p)
                    throw core::error_state;
                if (_margin.value.btop.unit == core::unit::per || _margin.value.bbottom.unit == core::unit::per)
                    throw core::error_bad_data;
                return value.value / 100.0f * (p->height() - calc_y(_margin.value.btop) - calc_y(_margin.value.bbottom));
            default:
                return value.value * s->ratio();
            }
        }
        core::vec2f calc(const core::vec2<core::dimensionf> & value, calc_flags flags = calc_flag::none) const
        {
            return { calc_x(value.x, flags), calc_y(value.y, flags) };
        }
        core::vec4f calc(const core::vec4<core::dimensionf> & value, calc_flags flags = calc_flag::none) const
        {
            return { calc(value.xy, flags), calc(value.zw, flags) };
        }

       core::rc32f box() const;
       core::rc32f controlBox() const { return box(); }
       core::rc32f borderBox() const;
       core::rc32f paddingBox() const;
       core::rc32f contentBox() const;
       core::rc32f box(control_box box) const;

        void refresh();
        void rearrange();
        void restyle();

        void setBackgroundColor(core::color32 color) { if (!_background_color.available() || _background_color != color) { _background_color = color; refresh(); } }
        core::color32 backgroundColor() const { return _background_color; }

        void setBackgroundImage(std::shared_ptr<graphics::Image> image);
        std::shared_ptr<graphics::Image> backgroundImage() const;

        void setMargin(const core::vec4<core::dimensionf> & margin) { if (_margin != margin) { _margin = margin; rearrange(); } }
        const core::vec4<core::dimensionf> & margin() const { return _margin; }

        void setBorder(const core::vec4<core::dimensionf> & border) { if (!_border.available() || _border != border) { _border = border; refresh(); } }
        const core::vec4<core::dimensionf> & border() const { return _border; }

        void setPadding(const core::vec4<core::dimensionf> & padding) { if (_padding != padding) { _padding = padding; rearrange(); } }
        const core::vec4<core::dimensionf> & padding() const { return _padding; }

        void setBorderColors(const core::vec4<core::color32> & boderColors) { _border_colors = boderColors; }
        const core::vec4<core::color32> & borderColors() const { return _border_colors; }
        void setBorderStyles(const core::vec4<graphics::stroke_style> & boderStyles) { _border_styles = boderStyles; }
        const core::vec4<core::color32> & boderColors() const { return _border_colors; }
        std::array<core::pt32f, 4> boderPoints(core::align edge) const;
        std::array<core::pt32f, 2> boderLine(core::align edge) const;

        bool acceptClip() const { return _accept_clip; }
        void setMouseThrough(bool b) { _mouse_through = b; }
        bool mouseThrough() const { return _mouse_through; }
        void setImeMode(ime_mode mode) { _ime_mode = mode; }
        ime_mode imeMode() const { return _ime_mode; }

        virtual void enteringScene(std::shared_ptr<Scene> & scene);
        virtual void enterScene(std::shared_ptr<Scene> & scene);
        virtual void leavingScene();
        virtual void leaveScene();

        // rect 控件应该定位的范围
        // size 控件的预计尺寸
        virtual void place(const core::rc32f & rect, const core::si32f & size);
        virtual std::string styleName() const { return {}; }

        virtual void updateStyle();
        virtual void update();
        virtual void updateContent() {}

        virtual int32_t animate();

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
        mutable std::mutex _mtx;

        std::weak_ptr<Scene> _scene;
        std::weak_ptr<Control> _parent;

        int32_t _zvalue = ZVALUE_CONTENT;
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

        core::aligns _anchor_borders = core::align::leftTop;
        core::attribute<core::vec4<core::dimensionf>> _anchor;

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
        bool _style_transition = true;
        // 布局之后
        core::pt32f _location;
        core::rc32f _rect;

        std::shared_ptr<renderables::Image> _background_imgage_obj;
        std::shared_ptr<renderables::Rectangle> _background_rect_obj;
        std::shared_ptr<renderables::Rectangle> _border_rect_obj;
        std::array<std::shared_ptr<renderables::Line>, 4> _border_line_objs;

        // true if need update
        bool _delay_update = false;
        bool _delay_style = false;

        bool _clip_children = true;
        bool _accept_clip = true;
        bool _interactable = true;
        bool _mouse_through = false;

        ime_mode _ime_mode = ime_mode::disabled;

        core::float3x2 _transform;
        std::multimap<int32_t, std::shared_ptr<component::Renderable>> _renderables;

        std::map<std::string, std::vector<std::shared_ptr<core::animation>>> _animations;
        core::rc32f _rect_invalid;

   public:
       void invalidate(const core::rc32f & rect);

       void clear();
       void insert(std::shared_ptr<component::Component> object) { insert(LOCAL_DEPTH_CONTENT, object); }
       void insert(int32_t depth, std::shared_ptr<component::Component> object);
       void remove(std::shared_ptr<component::Component> object);

       virtual void render(graphics::Graphics & graphics, const graphics::Region & region) const;

       virtual std::shared_ptr<Control> findChild(const core::pt32f & pos, std::shared_ptr<Control> last = nullptr) const { return nullptr; }

    public:
        void clearAnimations() { _animations.clear(); }
        void clearAnimations(std::string group);
        void appendAnimation(std::shared_ptr<core::animation> animation) { appendAnimation(std::string(), animation); }
        void appendAnimation(std::string group, std::shared_ptr<core::animation> animation);
    private:
        void _onAnimationStarted();


        //---------------------------------------------------- interact
    public:
        virtual hittest_result hitTest(const core::pt32f & pos) const
        {
            if (!_rect.contains(pos))
                return hittest_result::nowhere;

            if (_mouse_through)
                return hittest_result::transparent;

            return _interactable ? hittest_result::client : hittest_result::stable;
        }
        virtual void onMouseEnter(const mosue_state & state) { _mousein = true;  mouseEnter(state); }
        virtual void onMouseMove(const mosue_state & state) { mouseMove(state); }
        virtual void onMouseLeave(const mosue_state & state) { _pressed = false;  _mousein = false; mouseLeave(state); }

        virtual void onMouseDown(const mosue_state & state) { _pressed = true;  mouseDown(state); }
        virtual void onMouseUp(const mosue_state & state) { _pressed = false;  mouseUp(state); }
        virtual void onMouseClick(const mosue_state & state) { mouseClick(state); }
        virtual void onMouseDBClick(const mosue_state & state) { mouseDBClick(state); }
        virtual void onMouseWheel(const mosue_state & state) { mouseWheel(state); }
        virtual void onFocus(std::shared_ptr<ImeContext> imecontext) { _focused = true;  focus(); }
        virtual void onBlur() { _focused = false; blur(); }
        virtual void onChar(char32_t ch) {}

        bool mousein() const { return _mousein; }
        bool pressed() const { return _pressed; }

        void setCaptureButtons(mouse_buttons buttons) { _capture_buttons = buttons; }
        mouse_buttons captureButtons() const { return _capture_buttons; }

        void setAcceptWheelV(bool b) { _accept_wheel_v = b; }
        bool acceptWheelV() const { return _accept_wheel_v; }

        void setAcceptInput(bool b) { _accept_input = b; }
        bool acceptInput() const { return _accept_input; }

    public:
        core::event<void(const mosue_state & state)> mouseEnter;
        core::event<void(const mosue_state & state)> mouseMove;
        core::event<void(const mosue_state & state)> mouseLeave;
        core::event<void(const mosue_state & state)> mouseDown;
        core::event<void(const mosue_state & state)> mouseUp;
        core::event<void(const mosue_state & state)> mouseClick;
        core::event<void(const mosue_state & state)> mouseDBClick;
        core::event<void(const mosue_state & state)> mouseWheel;
        core::event<void()> focus;
        core::event<void()> blur;

    protected:
        bool _mousein = false;
        bool _pressed = false;
        bool _focused = false;
        mouse_buttons _capture_buttons = mouse_button::none;
        bool _accept_wheel_v = false;
        bool _accept_input = false;
    };
}
