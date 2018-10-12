#pragma once

namespace ui
{
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

    template<>
    struct enable_bitmasks<layout_flag>
    {
        static const bool enable = true;
    };

    typedef core::bitflag<layout_flag> layout_flags;

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
    template<>
    struct enable_bitmasks<findchild_flag>
    {
        static const bool enable = true;
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
        a,
        b,
        c,
        d,
        e,
        f,
        g,
        h,
        i,
        j,
        k,
        l,
        m,
        n,
        o,
        p,
        q,
        r,
        s,
        t,
        u,
        v,
        w,
        x,
        y,
        z,

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
        f1,
        f2,
        f3,
        f4,
        f5,
        f6,
        f7,
        f8,
        f9,
        f10,
        f11,
        f12,
        f13,
        f14,
        f15,
        f16,
        f17,
        f18,
        f19,
        f20,
        f21,
        f22,
        f23,
        f24,

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
        winL,
        winR,
        // -- ctrl
        control,
        controlL,
        controlR,
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

    template<>
    struct enable_bitmasks<mouse_button>
    {
        static const bool enable = true;
    };

    typedef core::bitflag<mouse_button> mouse_buttons;

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
        resizelbrt , // 斜向上调整大小
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

    class input_state
    {
    public:
        input_state() = default;
        input_state(const core::pt32f pos) : _pos(pos) {}

        void setPos(const core::pt32f & pos) { _pos = pos; }
        const core::pt32f & pos() const { return _pos; }

        void setButton(mouse_button button, bool active) { _buttons.set(button, active); }
        mouse_buttons buttons() const { return _buttons; }
        bool button(mouse_button button) const { return _buttons.any(button); }

        void setWheelLines(int32_t lines) { _wheel_lines = lines; }
        int32_t wheelLines() const { return _wheel_lines; }

        void setKey(keycode key, bool active) { _keys[(uint8_t)key] = active; }
        void setAllKeys(bool active) { for (size_t index = 0; index < 0xff; ++index) _keys[index] = active; }
        bool key(keycode key) const { return _keys[(uint8_t)key]; }

    private:
        core::pt32f _pos;
        mouse_buttons _buttons = mouse_button::none;
        int32_t _wheel_lines = 0;
        std::bitset<0xff> _keys;
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

    class ImeContext
    {
    public:
        virtual ~ImeContext() {}
        virtual void setImeMode(ui::ime_mode mode) = 0;
        virtual void setCompositionPos(core::pt32f pos) = 0;
        virtual void setCompositionFont(const drawing::font & font) = 0;
    };

    class CursorContext
    {
    public:
        virtual ~CursorContext() {}
        virtual void setCursor(cursor c) = 0;
        virtual void resetCursor() = 0;
    };
}
