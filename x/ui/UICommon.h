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
        // �ؼ��߿�
        layout_box = 0,
        // �߿򷽿�
        border_box,
        // �ڿ򷽿�
        padding_box,
        // ���ݷ���
        content_box,
    };

    enum class hittest_result
    {
        nowhere = 0,
        // ���λ�ÿɽ���
        client,
        // ���λ�ò��ɽ���
        stable,
        // �����Դ����λ�ô�͸��ȥ
        transparent,
    };

    enum class layout_origin
    {
        // Ĭ��λ��
        layout = 0,
        // ����� parent ��λ��
        parent,
        // ����� scene ��λ��
        scene,
        // ͬ layout��������� scene ���� parent ����
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
    * ���̰�����
    */
    enum class keycode : uint8_t
    {
        //! �����õİ����롣
        none = 0x0,

        // ���ּ�
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
        // �ַ���
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

        // С����
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

        // ���ܼ�
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
        sub, // ���� -_
        equal, // ���� =+
        grave, // ������ `~
        comma, // ���� ��<
        period, // ��� .>
        slash, // б�� /?
        semicolon, // �ֺ� ;:
        colon, // ð�� : (ĳЩ�����е�����ð��)
        apostrophe, // �����š�ʡ�ַ� '
        bracketL, // ������ [{
        bracketR, // �ҷ����� ]}
        backslash, // ��б�� \|

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

        // �����
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
        //! ����
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

        // -- ����
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
        unknown = -1, // δ֪
        normal = 0, // ��ͷ
        cross, // ʮ�ּ�
        hand, // ����
        help, // ����
        wait, // �ȴ�
        ibeam, // ������
        notaviliable, // ��Ч

        resizelr, // ���ҵ�����С
        resizetb, // ���µ�����С
        resizeltrb, // б���µ�����С
        resizelbrt , // б���ϵ�����С
        resizeall, // ȫ���������С

        user = 0xffff, // �û��Զ���
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
