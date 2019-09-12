#pragma once

#include "component/Style.h"

namespace ui
{
	class Form;

	enum class image_fitting
	{
		none = 0,
		scale,
		repeat,
	};

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

	enum class hittest_form
	{
		nowhere = 0,
		client,
		caption,
		resize_leftTop,
		resize_top,
		resize_rightTop,
		resize_right,
		resize_rightBottom,
		resize_bottom,
		resize_leftBottom,
		resize_left,
		close,
		minimize,
		maximize,
	};

	enum class form_style
	{
		normal = 0,
		frameless = 0x0001,
		layered = frameless | 0x0002,

		nomin = 0x100000,
		nomax = 0x200000,
		noclose = 0x400000,

		notaskbutton = 0x1000000,
		noresizable = 0x2000000,
		topmost = 0x4000000,

		mask_type = 0x000f,
	};
	typedef core::bitflag<form_style> form_styles;

	enum class form_state
	{
		hide = 0,
		noactive,
		normalize,
		minimize,
		maximize,
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

	enum class mouse_button
	{
		none = 0,
		left = 0x0001,
		middle = 0x0002,
		right = 0x0004,
		mask = 0x00ff,
	};
	typedef core::bitflag<mouse_button> mouse_buttons;

	enum class check_state
	{
		unchecked = 0,
		checked,
		unknown,
	};

	enum class findchild_flag
	{
		none = 0,
		disalbe_transparent = 0x0001,
	};
	typedef core::bitflag<findchild_flag> findchild_flags;

	/**
	* 键盘按键。
	*/
	enum class keycode : uint8_t
	{
		//! 不可用的按键码。
		none = 0x0,

		// 数字键
		num0,
		num1,
		num2,
		num3,
		num4,
		num5,
		num6,
		num7,
		num8,
		num9,
		// 字符键
		caps,
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,

		backspace,
		tab,
		enter,

		escape,
		space,

		// 小键盘
		numpad_lock,
		numpad_0,
		numpad_1,
		numpad_2,
		numpad_3,
		numpad_4,
		numpad_5,
		numpad_6,
		numpad_7,
		numpad_8,
		numpad_9,
		numpad_add, // +
		numpad_sub, // -
		numpad_mul, // *
		numpad_div,  // /
		numpad_decimal, // .
		numpad_enter, // enter
		numpad_equal, // equal

		// 功能键
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		F13,
		F14,
		F15,
		F16,
		F17,
		F18,
		F19,
		F20,
		F21,
		F22,
		F23,
		F24,

		// --  Sign
		sub, // 减号 -_
		equal, // 等于 =+
		grave, // 抑音符 `~
		comma, // 逗号 ，<
		period, // 句号 .>
		slash, // 斜杠 /?
		semicolon, // 分号 ;:
		colon, // 冒号 : (某些键盘有单独的冒号)
		apostrophe, // 单引号、省字符 '
		bracketL, // 左方括号 [{
		bracketR, // 右方括号 ]}
		backslash, // 反斜杠 \|

		insert,
		del,
		home,
		end,
		pageup,
		pagedown,

		left,
		up,
		right,
		down,

		printscreen,
		scrolllock,
		pausebreak,

		// -- win
		win,
		winL,
		winR,
		// -- ctrl
		ctrl,
		ctrlL,
		ctrlR,
		// -- shift
		shift,
		shiftL,
		shiftR,
		// -- alt
		alt,
		altL,
		altR,
		// -- app
		apps,

		// 特殊键
		sleep,

		clear,
		select,
		print,
		excute,
		help,

		// -- ime
		ime_kana,
		ime_hangul,
		ime_junja,
		ime_final,
		ime_hanja,
		ime_kanji,
		ime_convert,
		ime_nonconvert,
		ime_accept,
		ime_modechange,
		ime_processkey,

		// -- browser
		browser_back,
		browser_forward,
		browser_refresh,
		browser_stop,
		browser_search,
		browser_favorites,
		browser_home,

		// -- volume
		//! 静音
		volume_mute,
		volume_down,
		volume_up,

		// -- media
		media_next,
		media_prev,
		media_stop,
		media_playpause,

		// -- launch
		launch_mail,
		launch_mediaselect,
		launch_app1,
		launch_app2,

		// -- 其他
		else_attn,
		else_arsel,
		else_exsel,
		else_ereof,
		else_play,
		else_zoom,
		else_pa1,
		else_clear,

		count,
	};

	const char8_t * keycode_name(keycode k);

	enum class cursor
	{
		unknown = -1, // 未知
		normal = 0, // 箭头
		cross, // 十字架
		hand, // 手型
		help, // 帮助
		wait, // 等待
		ibeam, // 工字型
		notaviliable, // 无效

		resizelr, // 左右调整大小
		resizetb, // 上下调整大小
		resizeltrb, // 斜向下调整大小
		resizelbrt, // 斜向上调整大小
		resizeall, // 全方向调整大小

		user = 0xffff, // 用户自定义
	};

	enum class mouse_action
	{
		none = 0,
		enter,
		move,
		press,
		click,
		release,
		dbclick,
		leave,

		wheel_v,
	};

	enum class scene_event
	{
		none = 0,
		update_mouse_pos,
	};

	enum class system_action : uint32_t
	{
		none = 0,
		cut,
		copy,
		paste,

		close,
		minimize,
		maximize,
	};

	union action_t
	{
		action_t() :e(system_action::none) {}
		action_t(system_action e_) :e(e_) {}
		action_t(uint32_t ui_) : ui(ui_) {}

		operator uint32_t() const { return ui; }

		system_action e;
		uint32_t ui;
	};

	class input_state
	{
	public:
		input_state() = default;
		input_state(const core::pointf pos) : _pos(pos) {}

		void setPos(const core::pointf & pos) { _pos = pos; }
		const core::pointf & pos() const { return _pos; }

		void setButton(mouse_button button, bool active) { _buttons.set(button, active); }
		mouse_buttons buttons() const { return _buttons; }
		bool button(mouse_button button) const { return _buttons.any(button); }

		void setHoving(bool h) { _hoving = h; }
		bool hoving() const { return _hoving; }

		void setWheelLines(int32_t lines) { _wheel_lines = lines; }
		int32_t wheelLines() const { return _wheel_lines; }

		void setKey(keycode key, bool active) { _keys[(uint8_t)key] = active; }
		void setAllKeys(bool active) { for (size_t index = 0; index < 0xff; ++index) _keys[index] = active; }
		bool key(keycode key) const { return _keys[(uint8_t)key]; }

		void setFocused(bool f) { _focused = f; }
		bool focused() const { return _focused; }

	private:
		core::pointf _pos;
		mouse_buttons _buttons = mouse_button::none;
		bool _hoving = false;
		int32_t _wheel_lines = 0;
		std::bitset<0xff> _keys;
		bool _focused = false;
	};

	enum class key_action
	{
		none = 0,
		press,
		release,
	};

	enum class ime_mode
	{
		disabled = 0,
		on,
		off,
	};

	enum class keybind_mode
	{
		// 完全按照键顺序绑定
		exact = 0,

	};

	struct keybind
	{
		static constexpr size_t max_keys = 4;
		std::array<keycode, max_keys> keys = {};

		keybind() = default;
		keybind(std::initializer_list<keycode> && ks)
		{
			size_t index = 0;
			for (auto & k : ks)
			{
				keys[index++] = k;
				if (index >= max_keys)
					break;
			}
		}
		bool empty() const { return keys[0] == keycode::none; }
		std::u8string string() const;
	};

	struct shortcut
	{
		static constexpr size_t max_binds = 2;
		std::array<keybind, max_binds> binds;

		shortcut() = default;
		shortcut(std::initializer_list<keybind> && bs)
		{
			size_t index = 0;
			for (auto & b : bs)
			{
				binds[index++] = b;
				if (index >= max_binds)
					break;
			}
		}
		bool empty() const { return binds[0].empty(); }
		std::u8string string() const;
	};

	class IImeContext
	{
	public:
		virtual ~IImeContext() {}
		virtual void setImeMode(ui::ime_mode mode) = 0;
		virtual void setCompositionPos(core::pointf pos) = 0;
		virtual void setCompositionFont(const drawing::font & font) = 0;
	};

	class ICursorContext
	{
	public:
		virtual ~ICursorContext() {}
		virtual void setCursor(cursor c) = 0;
		virtual void resetCursor() = 0;
	};

	class IWindow
	{
	public:
		virtual ~IWindow() {}

		virtual core::error attatch(std::shared_ptr<ui::Form> form) = 0;
		virtual pointer_t handle() const = 0;
		virtual std::shared_ptr<IImeContext> imeContext() const = 0;
		virtual std::shared_ptr<ICursorContext> cursorContext() const = 0;
		virtual void move(const core::pointf & pos) = 0;
		virtual void resize(const core::sizef & size) = 0;
		virtual void show(form_state state) = 0;
	};

	enum class item_flag
	{
		none = 0,
		marked = 0x0001,
		activated = 0x0002,
		selected = 0x0004,
		focused = 0x0008,
		disabled = 0x0010,

		shown = 0x1000,

		placed = 0x10000,
	};
	typedef core::bitflag<item_flag> item_flags;

	class Item
	{
	public:
		virtual ~Item() {}
	};

	class MenuItem : public Item
	{
	public:
		MenuItem() {}
		MenuItem(const std::u8string & i, const std::u8string & t, ui::shortcut s, action_t a = 0, item_flags f = nullptr) : icon(i), text(t), shortcut(s.string()), action(a), flags(f) {}

		std::u8string icon;
		std::u8string text;
		std::u8string shortcut;
		action_t action = 0;

		item_flags flags;

		core::event<void(action_t action)> active;
	};

	class IMenuPresenter
	{
	public:
		virtual ~IMenuPresenter() {}

		virtual size_t appendItem(std::shared_ptr<MenuItem> item) = 0;
		virtual size_t appendItems(std::initializer_list<std::shared_ptr<MenuItem>> && items) = 0;
		virtual size_t insertItem(size_t index, std::shared_ptr<MenuItem> item) = 0;
	};
}
