#pragma once

#include "component/Style.h"
#include "drawing/Region.h"
#include "UICommon.h"
#include "Scene.h"

namespace ui
{
    const int32_t ZVALUE_BACKGROUND = -100;
    const int32_t ZVALUE_CONTENT = 0;
    const int32_t ZVALUE_FOREGROUND = 100;
    const int32_t ZVALUE_TOPMOST = 0xFFFFFFF;
    const int32_t ZVALUE_MAX = 0xFFFFFFF;

    const int32_t ZVALUE_SCROLLBAR = 0x10000001;

    const int32_t LOCAL_DEPTH_BACKGROUND = -100;
    const int32_t LOCAL_DEPTH_CONTENT = 0;
    const int32_t LOCAL_DEPTH_FOREGROUND = 100;

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
        void setFont(const drawing::font & font);
        const drawing::font & font() const;

        void setVisible(bool vis)
        {
            if (_visible != vis)
            {
                _visible = vis;
                onVisibleChanged(vis);
            }
        }

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
                return value.value * drawing::fontmetrics(drawing::font()).height;
            case core::unit::pt:
                return value.value * 72.0f * s->ratio();
            case core::unit::dot:
                return value.value;
            case core::unit::per:
                if (flags.any(calc_flag::donot_calc_percent_x))
                    return 0;
                if (!p)
                    throw core::error_state;
                if (_margin.value.bleft.unit == core::unit::per || _margin.value.bright.unit == core::unit::per)
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
                return value.value * drawing::fontmetrics(drawing::font()).height;
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
        void refresh(const core::rc32f & rect);
        void rearrange();
        void restyle();

        void setBackgroundColor(core::color32 color)
        {
            if (!_background_color.available() || _background_color != color)
            {
                _background_color = color;
                refresh();
            }
        }

        core::color32 backgroundColor() const { return _background_color; }

        void setBackgroundImage(std::shared_ptr<drawing::Image> image);
        std::shared_ptr<drawing::Image> backgroundImage() const;

        void setMargin(const core::vec4<core::dimensionf> & margin)
        {
            if (_margin != margin)
            {
                _margin = margin;
                rearrange();
            }
        }

        const core::vec4<core::dimensionf> & margin() const { return _margin; }

        void setBorder(const core::vec4<core::dimensionf> & border)
        {
            if (!_border.available() || _border != border)
            {
                _border = border;
                refresh();
            }
        }

        const core::vec4<core::dimensionf> & border() const { return _border; }

        void setPadding(const core::vec4<core::dimensionf> & padding)
        {
            if (_padding != padding)
            {
                _padding = padding;
                rearrange();
            }
        }

        const core::vec4<core::dimensionf> & padding() const { return _padding; }

        void setBorderColors(const core::vec4<core::color32> & boderColors) { _border_colors = boderColors; }
        const core::vec4<core::color32> & borderColors() const { return _border_colors; }
        void setBorderStyles(const core::vec4<drawing::stroke_style> & boderStyles) { _border_styles = boderStyles; }
        const core::vec4<core::color32> & boderColors() const { return _border_colors; }
        std::array<core::pt32f, 4> boderPoints(core::align edge) const;
        std::array<core::pt32f, 2> boderLine(core::align edge) const;

        void setAlpha(float32_t alpha) { _alpha = alpha; }
        int32_t alpha() const { return _alpha; }
        bool acceptClip() const { return _accept_clip; }
        void setMouseThrough(bool b) { _mouse_through = b; }
        bool mouseThrough() const { return _mouse_through; }
        void setImeMode(ime_mode mode) { _ime_mode = mode; }
        ime_mode imeMode() const { return _ime_mode; }
        void setCursor(cursor cs) { _cursor = cs; }
        cursor cursor() const { return _cursor; }

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

        virtual int32_t animate();

        virtual void ondraw(drawing::Graphics & graphics, const drawing::Region & region) const;
        virtual void draw(drawing::Graphics & graphics, const drawing::Region & region) const;

        virtual std::shared_ptr<Control> findChild(const core::pt32f & pos, std::shared_ptr<Control> last = nullptr) const { return nullptr; }

    public:
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
        void _drawBackground(drawing::Graphics & graphics) const;
        void _drawBorder(drawing::Graphics & graphics) const;
        void _adjustSizeMinMax(core::si32f & size) const;

    protected:
        mutable std::mutex _mtx;

        std::weak_ptr<Scene> _scene;
        std::weak_ptr<Control> _parent;

        int32_t _zvalue = ZVALUE_CONTENT;
        layout_origin _layout_origin = layout_origin::layout;

        core::attribute<core::color32> _color = core::colors::Auto;
        // 控件大小，包括 padding，不包括 margin。
        core::attribute<core::vec2<core::dimensionf>> _pos;
        core::attribute<core::vec2<core::dimensionf>> _size;
        core::attribute<core::vec4<core::dimensionf>> _padding;
        core::attribute<core::vec4<core::dimensionf>> _border;
        core::attribute<core::vec4<core::color32>> _border_colors;
        core::attribute<core::vec4<drawing::stroke_style>> _border_styles;
        core::attribute<core::vec4<core::dimensionf>> _margin;
        core::attribute<core::vec2<core::dimensionf>> _minSize;
        core::attribute<core::vec2<core::dimensionf>> _maxSize;

        core::aligns _anchor_borders = core::align::leftTop;
        core::attribute<core::vec4<core::dimensionf>> _anchor;

        core::attribute<drawing::font> _font;

        core::attribute<bool> _visible = true;
        // background
        core::attribute<std::shared_ptr<drawing::Image>> _background_image;
        core::attribute<core::color32> _background_color = core::colors::Auto;
        layout_origin _background_position;
        core::attribute<core::vec2<core::dimensionf>> _background_size;
        core::vec2<image_fitting> _background_fitting = core::vec2<image_fitting>(image_fitting::none, image_fitting::none);
        control_box _background_box = control_box::layout_box;

        std::shared_ptr<component::StyleSheet> _styleSheet;
        std::string _style;
        bool _style_transition = true;
        // 布局之后
        core::pt32f _location;
        core::rc32f _rect;


        // true if need update
        bool _delay_update = false;
        bool _delay_style = false;
        enum class cursor _cursor = cursor::unknown;
        float32_t _alpha = 1.0f;

        bool _clip_children = true;
        bool _accept_clip = true;
        bool _interactable = true;
        bool _mouse_through = false;

        ime_mode _ime_mode = ime_mode::disabled;

        core::float3x2 _transform;

        std::map<std::string, std::vector<std::shared_ptr<core::animation>>> _animations;
        core::rc32f _rect_invalid;

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

        virtual void onMouseEnter(const input_state & state);
        virtual void onMouseMove(const input_state & state) { mouseMove(state); }
        virtual void onMouseLeave(const input_state & state);

        virtual void onMouseDown(const input_state & state, mouse_button button)
        {
            if (button == mouse_button::left)
                _pressed = true;
            mouseDown(state, button);
        }

        virtual void onMouseUp(const input_state & state, mouse_button button)
        {
            if (button == mouse_button::left)
                _pressed = false;
            mouseUp(state, button);
        }

        virtual void onMouseClick(const input_state & state, mouse_button button) { mouseClick(state, button); }
        virtual void onMouseDBClick(const input_state & state, mouse_button button) { mouseDBClick(state, button); }
        virtual void onMouseWheel(const input_state & state) { mouseWheel(state); }

        virtual void onFocus(std::shared_ptr<ImeContext> imecontext)
        {
            _focused = true;
            focus();
        }

        virtual void onBlur()
        {
            _focused = false;
            blur();
        }

        virtual void onKeyDown(const input_state & state, keycode key) {}
        virtual void onKeyUp(const input_state & state, keycode key) {}
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
        core::event<void(const input_state & state)> mouseEnter;
        core::event<void(const input_state & state)> mouseMove;
        core::event<void(const input_state & state)> mouseLeave;
        core::event<void(const input_state & state, mouse_button button)> mouseDown;
        core::event<void(const input_state & state, mouse_button button)> mouseUp;
        core::event<void(const input_state & state, mouse_button button)> mouseClick;
        core::event<void(const input_state & state, mouse_button button)> mouseDBClick;
        core::event<void(const input_state & state)> mouseWheel;
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
