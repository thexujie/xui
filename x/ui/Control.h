#pragma once

#include "component/Style.h"
#include "drawing/Region.h"
#include "UICommon.h"

namespace ui
{
    class Container;
    class Form;

    const int32_t ZVALUE_BACKGROUND = -100;
    const int32_t ZVALUE_CONTENT = 0;
    const int32_t ZVALUE_FOREGROUND = 100;
    const int32_t ZVALUE_TOPMOST = 0xFFFFFFF;
    const int32_t ZVALUE_MAX = 0xFFFFFFF;

    const int32_t ZVALUE_SCROLLBAR = 0x10000001;

    const int32_t LOCAL_DEPTH_BACKGROUND = -100;
    const int32_t LOCAL_DEPTH_CONTENT = 0;
    const int32_t LOCAL_DEPTH_FOREGROUND = 100;

    const std::string CONTROL_ANIMATION_GROUP_STYLE = "_style";
    const std::string CONTROL_ANIMATION_GROUP_CONTROL = "_control";

    class Control : public core::object
    {
    public:
        Control();
        virtual ~Control();

        std::shared_ptr<Control> control_ref() const { return const_cast<Control *>(this)->share_ref<Control>(); }
        static void propertyTableCallback(core::property_table & properties);
        virtual void propertyTable(core::property_table & properties);
        virtual const core::property_table & properties();

        void setZValue(int32_t zvalue) { _zvalue = zvalue; }
        int32_t ZValue() const { return _zvalue; }

        void setLayoutOrigin(layout_origin origin) { _layout_origin = origin; }
        layout_origin layoutOrigin() const { return _layout_origin; }

        void setPos(const core::vec2<core::dimenf> & pos) { _pos = pos; }
        const core::vec2<core::dimenf> & pos() const { return _pos; }
        void setSize(const core::vec2<core::dimenf> & size) { _size = size; }
        const core::vec2<core::dimenf> & size() const { return _size; }
        void setMinSize(const core::vec2<core::dimenf> & minSize) { _min_size = minSize; }
        const core::vec2<core::dimenf> & minSize() const { return _min_size; }
        void setMaxSize(const core::vec2<core::dimenf> & maxSize) { _max_size = maxSize; }
        const core::vec2<core::dimenf> & maxSize() const { return _max_size; }
        void setFixedAspect(const core::vec2<float32_t> & maxSize) { _fixed_aspect = maxSize; }
        const core::vec2<float32_t> & fixedAspect() const { return _fixed_aspect; }

        void setPlaceAlignment(core::aligns align) { _place_alignment = align; }
        core::aligns placeAlignment() const { return _place_alignment; }
        void setAnchor(const core::vec4<core::dimenf> & anchor) { _anchor = anchor; }
        const core::vec4<core::dimenf> & anchor() const { return _anchor; }

        void move(const core::vec2<core::dimenf> & pos);
        void resize(const core::vec2<core::dimenf> & size);

        void setShowPos(const core::vec2f & pos);
        void setShowSize(const core::vec2f & size);
        void setShowRect(const core::rectf & size);

        // prefferSize 计算出的期望大小
        core::sizef prefferSize() const;
        core::sizef anchorSize(const core::sizef & spacing) const;
        // 根据最大最小值调整
        core::sizef adjustSize(const core::sizef & size) const;

        virtual core::sizef fittingSize(const core::sizef & spacing) const { return { std::nanf(0) , std::nanf(0) }; }
        virtual core::sizef contentSize() const { return core::sizef(); }

        void setParent(std::shared_ptr<Container> parent) { _parent = parent; }
        std::shared_ptr<Container> parent() const { return _parent.lock(); }

        virtual std::shared_ptr<Form> form() const;
        virtual std::shared_ptr<component::StyleSheet> styleSheet() const;
        virtual std::shared_ptr<ui::IImeContext> imeContext() const;
        virtual std::shared_ptr<ICursorContext> cursorContext() const;

        virtual float_t ratio() const;
        void setColor(core::color color) { _color = color; }
        const core::color & color() const;
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
		bool visible() const { return _visible; }

		void setAviliable(bool avi)
		{
			if(_aviliable != avi)
			{
				_aviliable = avi;
				onAviliableChanged(avi);
			}
		}
		bool aviliable() const { return _aviliable; }

        core::pointf realPos() const { return _rect.pos; }
        core::sizef realSize() const { return _rect.size; }
        float32_t realWidth() const { return _rect.cx; }
        float32_t realHeight() const { return _rect.cy; }
        core::rectf realRect() const { return _rect; }
        float32_t width() const { return _rect.cx; }
        float32_t height() const { return _rect.cy; }
        core::vec4f realMargin() const { return calc(_margin); }

        float32_t calc(const core::dimenf & value) const;
        float32_t calc(const core::dimenf & value, float32_t spacing) const;
        core::vec2f calc(const core::vec2<core::dimenf> & value) const
        {
            return { calc(value.x), calc(value.y) };
        }
        core::vec2f calc(const core::vec2<core::dimenf> & value, const core::vec2<float32_t> & spacing) const
        {
            return { calc(value.x, spacing.cx), calc(value.y, spacing.cy) };
        }
        core::vec4f calc(const core::vec4<core::dimenf> & value) const
        {
            return { calc(value.xy), calc(value.zw) };
        }
        core::vec4f calc(const core::vec4<core::dimenf> & value, const core::vec2<float32_t> & spacing) const
        {
            return { calc(value.x, spacing.cx), calc(value.y, spacing.cy), calc(value.cx, spacing.cx), calc(value.cy, spacing.cy) };
        }

        core::rectf box() const;
        core::rectf controlBox() const { return box(); }
        core::rectf borderBox() const;
        core::rectf paddingBox() const;
        core::rectf contentBox() const;
        core::rectf box(control_box box) const;

        void refresh();
        void repaint();
        void repaint(const core::rectf & rect);
        void rearrange();
        void restyle();

        void setBackgroundColor(core::color color)
        {
            if (!_background_color.available() || _background_color != color)
            {
                _background_color = color;
                repaint();
            }
        }

        core::color backgroundColor() const { return _background_color; }

        void setBackgroundImage(std::shared_ptr<drawing::Image> image);
        std::shared_ptr<drawing::Image> backgroundImage() const;

        void setMargin(const core::vec4<core::dimenf> & margin);
        const core::vec4<core::dimenf> & margin() const { return _margin; }

        void setBorder(const core::vec4<core::dimenf> & border)
        {
            if (!_border.available() || _border != border)
            {
                _border = border;
                repaint();
            }
        }

        const core::vec4<core::dimenf> & border() const { return _border; }

        void setPadding(const core::vec4<core::dimenf> & padding)
        {
            if (_padding != padding)
            {
                _padding = padding;
                rearrange();
            }
        }

        const core::vec4<core::dimenf> & padding() const { return _padding; }
        core::vec4f edges() const;

        void setBorderColors(const core::vec4<core::color> & boderColors) { _border_colors = boderColors; }
        const core::vec4<core::color> & borderColors() const { return _border_colors; }
        void setBorderStyles(const core::vec4<drawing::stroke_style> & boderStyles) { _border_styles = boderStyles; }
        const core::vec4<core::color> & boderColors() const { return _border_colors; }
        std::array<core::pointf, 4> boderPoints(core::align edge) const;
        std::array<core::pointf, 2> boderLine(core::align edge) const;

        void setAlpha(float32_t alpha) { _alpha = alpha; }
        float32_t alpha() const { return _alpha; }
        void setMouseThrough(bool b) { _mouse_through = b; }
        bool mouseThrough() const { return _mouse_through; }
        void setImeMode(ime_mode mode) { _ime_mode = mode; }
        ime_mode imeMode() const { return _ime_mode; }
        void setCursor(cursor cs) { _cursor = cs; }
        cursor cursor() const { return _cursor; }

        virtual void onEntering(std::shared_ptr<Form> & form);
        virtual void onEnter(std::shared_ptr<Form> & form);
        virtual void onLeaving();
        virtual void onLeave();

        // rect 控件应该定位的范围
        // size 控件的预计尺寸
        virtual void place(const core::rectf & rect, const core::sizef & size);
        virtual std::string styleName() const { return {}; }

        virtual void setEvent(scene_event evt);

        virtual void valid() const;
        virtual void update() {}
        virtual void updateStyle();
        virtual void invalidate();
		virtual void invalidate(const core::rectf & rect);
		virtual bool validCompleted() const { return !_delay_repaint; }

        virtual void animate();
        virtual size_t onAnimate();

        virtual void onPaint(drawing::Graphics & graphics, const core::rectf & clip) const;
        virtual void paint(drawing::Graphics & graphics, const core::rectf & clip) const;

        virtual std::shared_ptr<Control> findChild(const core::pointf & pos, std::shared_ptr<Control> last = nullptr, findchild_flags flags = nullptr) const { return nullptr; }

    public:
        virtual void onPosChanged(const core::pointf & from, const core::pointf & to);
        virtual void onSizeChanged(const core::sizef & from, const core::sizef & to);
        virtual void onRectChanged(const core::rectf & from, const core::rectf & to);
		virtual void onVisibleChanged(bool vis);
		virtual void onAviliableChanged(bool avi);

    public:
        core::event<void(const core::pointf & from, const core::pointf & to)> posChanged;
        core::event<void(const core::sizef & from, const core::sizef & to)> sizeChanged;
        core::event<void(const core::rectf & from, const core::rectf & to)> rectChanged;
        core::event<void(bool vis)> visibleChanged;

    protected:
        void _drawBackground(drawing::Graphics & graphics) const;
        void _drawBorder(drawing::Graphics & graphics) const;
        void _adjustSize(core::sizef & size) const;
        void _adjustSize(core::sizef & size, const core::vec2<float32_t> & spacing) const;

    protected:
        std::weak_ptr<Container> _parent;

        int32_t _zvalue = ZVALUE_CONTENT;
        layout_origin _layout_origin = layout_origin::layout;

        core::attribute<core::color> _color = core::colors::Auto;
        // 控件大小，包括 padding，不包括 margin。
        core::attribute<core::vec2<core::dimenf>> _pos;
        core::attribute<core::vec2<core::dimenf>> _size;
        core::attribute<core::vec2<float32_t>> _fixed_aspect;
        core::attribute<core::vec4<core::dimenf>> _padding;
        core::attribute<core::vec4<core::dimenf>> _border;
        core::attribute<core::vec4<core::color>> _border_colors;
        core::attribute<core::vec4<drawing::stroke_style>> _border_styles;
        core::attribute<core::vec4<core::dimenf>> _margin;
        core::attribute<core::vec2<core::dimenf>> _min_size = core::vec2{ core::auto_value , core::auto_value };
        core::attribute<core::vec2<core::dimenf>> _max_size = core::vec2{ core::auto_value, core::auto_value };

        core::attribute<core::aligns> _place_alignment;
        core::vec4<core::dimenf> _anchor = { core::nanf32 };

        core::attribute<drawing::font> _font;

		bool _visible = true;
		bool _aviliable = true;
        // background
        core::attribute<std::shared_ptr<drawing::Image>> _background_image;
        core::attribute<core::color> _background_color = core::colors::Auto;
        layout_origin _background_position;
        core::attribute<core::vec2<core::dimenf>> _background_size;
        core::vec2<image_fitting> _background_fitting = core::vec2<image_fitting>(image_fitting::none, image_fitting::none);
        control_box _background_box = control_box::layout_box;

        std::string _style;
        bool _style_transition = true;
        // 布局之后
        core::pointf _location;
        core::rectf _rect;

        bool _delay_style = true;
        bool _delay_update = true;
        bool _delay_repaint = false;
        enum class cursor _cursor = cursor::unknown;
        float32_t _alpha = 1.0f;

        bool _interactable = true;
        bool _mouse_through = false;

        ime_mode _ime_mode = ime_mode::disabled;

        core::float3x2 _transform;

        std::map<std::string, std::vector<std::shared_ptr<core::animation>>> _animations;
        core::rectf _rect_repaint;

    public:
        void clearAnimations() { _animations.clear(); }
        void clearAnimations(std::string group);
        void appendAnimation(std::shared_ptr<core::animation> animation) { appendAnimation(std::string(), animation); }
        void appendAnimation(std::string group, std::shared_ptr<core::animation> animation);

        //---------------------------------------------------- interact
    public:
        virtual hittest_result hitTest(const core::pointf & pos) const
        {
            if (!_visible || !_rect.contains(pos))
                return hittest_result::nowhere;

            if (_mouse_through)
                return hittest_result::transparent;

            return _interactable ? hittest_result::client : hittest_result::stable;
        }

        virtual hittest_form hitTestForm(const core::pointf & pos) const
        {
            return (_mouse_through || !_interactable) ? hittest_form::caption : hittest_form::client;
        }

        virtual void onMouseEnter(const input_state & state);
        virtual void onMouseMove(const input_state & state) { onHover(state); }
        virtual void onMouseLeave(const input_state & state);

        virtual void onMouseDown(const input_state & state, mouse_button button)
        {
            if (button == mouse_button::left)
            {
                setActived(true);
                onActiveIn(state);
            }
        }

        virtual void onMouseUp(const input_state & state, mouse_button button)
        {
            if (button == mouse_button::left)
            {
                onActiveOut(state);
                setActived(false);
            }
        }

        virtual void onMouseClick(const input_state & state, mouse_button button) {  active(_action); onActive(state); }
        virtual void onMouseDBClick(const input_state & state, mouse_button button) { }
        virtual void onMouseWheel(const input_state & state) { onWheel(state); }

        virtual void onKeyDown(const input_state & state, keycode key) {}
        virtual void onKeyUp(const input_state & state, keycode key) {}
        virtual void onChar(char32_t ch) {}

        virtual void onHover(const input_state & state)
        {
            hover(state);
        }

        virtual void onWheel(const input_state & state)
        {
            wheel(state);
        }
		virtual void onPopupMenu(const input_state & state, IMenuPresenter & presenter) { }

        virtual void onHoverIn(const input_state & state){}
        virtual void onHoverOut(const input_state & state){}
        virtual void onActiveIn(const input_state & state){}
        virtual void onActive(const input_state & state){}
        virtual void onActiveOut(const input_state & state){}
        virtual void onFocusIn(){}
        virtual void onFocusOut(){}

        void setHovered(bool m) { if (_hovered != m) { _hovered = m; restyle(); hoveredChanged(_hovered); } }
        bool hovered() const { return _hovered; }

        template<typename T>
        void setActionT(T action) { setAction(static_cast<uintx_t>(action)); }
        void setAction(uintx_t action) { _action = action; }
        uintx_t action() const { return _action; }
        void setActived(bool a) { if (_actived != a) { _actived = a; restyle(); activeChanged(_actived); } }
        bool actived() const { return _actived; }

        void setFocused(bool f) { if (_focused != f) { _focused = f; restyle(); focusChanged(_focused); } }
        bool focused() const { return _focused; }

        void setCaptureButtons(mouse_buttons buttons) { _capture_buttons = buttons; }
        mouse_buttons captureButtons() const { return _capture_buttons; }

        void setAcceptWheelV(bool b) { _accept_wheel_v = b; }
        bool acceptWheelV() const { return _accept_wheel_v; }
        virtual core::aligns wheelFreedom() const { return core::align::none; }

        void setAcceptInput(bool b) { _accept_input = b; }
        bool acceptInput() const { return _accept_input; }

    public:
        core::event<void(const input_state & state)> hover;
        core::event<void(const input_state & state)> wheel;

        core::event<void(bool)> hoveredChanged;
        core::event<void(bool)> activeChanged;
        core::event<void(bool)> focusChanged;

        core::event<void(uintx_t action)> active;

    protected:
        bool _hovered = false;
        bool _actived = false;
        bool _focused = false;
        mouse_buttons _capture_buttons = mouse_button::none;
        bool _accept_wheel_v = false;
        bool _accept_input = false;

        uintx_t _action = 0;
    };
}
