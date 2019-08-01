#pragma once

namespace core
{
    template<typename T>
    T property_parser(const std::u8string & str);

    class property_value
    {
    public:
        virtual ~property_value() = default;
        virtual bool equal(const property_value & another) const = 0;
    };

    struct property_accessor
    {
        virtual ~property_accessor() = default;
        virtual void set(object & self, const property_value & val) = 0;
        virtual void get(object & self, property_value & val) = 0;

        virtual void set(const std::u8string & str, object & self) = 0;
        virtual void get(std::u8string & str, object & self) = 0;

        virtual void serialize(const std::u8string & str, property_value & value) const = 0;
        virtual void serialize(property_value & value, std::u8string & str) const = 0;

        virtual std::shared_ptr<property_value> serialize(const std::u8string & str) const = 0;
        virtual std::shared_ptr<class property_interpolator> create_interpolator() const = 0;
    };

    class property_interpolator
    {
    public:
        virtual ~property_interpolator() = default;
        virtual property_value & start() = 0;
        virtual property_value & end() = 0;
        virtual void interpolate(object & self, core::property_accessor & accesor, float32_t proportion) = 0;
        virtual void interpolate(core::property_value & value, float32_t inter) = 0;
    };

    template<typename T>
    class property_value_type : public property_value
    {
    public:
        property_value_type() = default;
        property_value_type(const T & val) : _val(val) {}

        void set(const T & val) { _val = val; }
        T & get() { return _val; }
        const T & get() const { return _val; }
        bool equal(const property_value & another) const
        {
            if (typeid(*this) != typeid(another))
                return false;

            const auto & a = dynamic_cast<const property_value_type<T> &>(another);
            return _val == a._val;
        }
    private:
        T _val;
    };

    template<typename T>
    struct property_accessor_type : public property_accessor
    {
        virtual void set_value(object & self, const T & val) = 0;
        virtual T get_value(object & self) = 0;
    };

    template <typename T, typename = void>
    struct is_interpolatorable : std::false_type {};

    template <typename T>
    struct is_interpolatorable<T, std::void_t<decltype((std::declval<T>() - std::declval<T>()) * float32_t(0))>> : std::true_type {};

    template<typename T>
    inline constexpr bool is_interpolatorable_v = is_interpolatorable<T>::value;
    
    template<typename T>
    T property_interpolator_calc(const T & s, const T & e, float32_t proportion)
    {
        if constexpr(core::is_interpolatorable_v<T>)
            return s + static_cast<T>((e - s) * proportion);
        else
            return proportion < 0.5f ? s : e;
    }

    template<typename T>
    class property_interpolator_default : public property_interpolator
    {
    public:
        property_interpolator_default() = default;
        property_value & start() { return _start; }
        property_value & end() { return _end; }

        void set_values(const T & start, const T & end)
        {
            _start.set(start);
            _end.set(end);
        }

        void interpolate(object & self, core::property_accessor & accesor, float32_t proportion)
        {
            T s = _start.get();
            T e = _end.get();
            auto & a = dynamic_cast<property_accessor_type<T> &>(accesor);
            a.set_value(self, property_interpolator_calc(s, e, proportion));
        }

        void interpolate(core::property_value & value, float32_t proportion)
        {
            T s = _start.get();
            T e = _end.get();
            auto & v = dynamic_cast<property_value_type<T> &>(value);
            v.set(property_interpolator_calc(s, e, proportion));
        }

    private:
        property_value_type<T> _start;
        property_value_type<T> _end;
    };

    template<typename ClassT, typename T, typename SetterT, typename GetterT>
    struct property_accessor_impl : public property_accessor_type<T>
    {
        property_accessor_impl(SetterT setter_, GetterT getter_,
            std::function<T(const std::u8string &)> store, std::function<std::u8string(const T &)> fetch) : setter(setter_), getter(getter_), _store(store), _fetch(fetch) {}

        SetterT setter;
        GetterT getter;
        std::function<T(const std::u8string &)> _store;
        std::function<std::u8string(const T &)> _fetch;

        void set(object & self, const property_value & val)
        {
            auto & s = dynamic_cast<ClassT &>(self);
            auto & v = dynamic_cast<const property_value_type<T> &>(val);
            (s.*setter)(v.get());
        }

        void get(object & self, property_value & val)
        {
            auto & s = dynamic_cast<ClassT &>(self);
            auto & v = dynamic_cast<property_value_type<T> &>(val);
            if constexpr(std::is_member_object_pointer_v<GetterT>)
                v.set(s.*getter);
            else
                v.set((s.*getter)());
        }

        void set_value(object & self, const T & val)
        {
            auto & s = dynamic_cast<ClassT &>(self);
            (s.*setter)(val);
        }

        T get_value(object & self)
        {
            auto & s = dynamic_cast<ClassT &>(self);
            if constexpr (std::is_member_object_pointer_v<GetterT>)
                return s.*getter;
            else
                return (s.*getter)();
        }


        void get(std::u8string & str, object & self)
        {
            str = _fetch(get_value(self));
        }

        void set(const std::u8string & str, object & self)
        {
            set_value(self, _store(str));
        }

        void serialize(const std::u8string & str, property_value & value) const
        {
            auto & v = dynamic_cast<property_value_type<T> &>(value);
            v.set(_store(str));
        }

        void serialize(property_value & value, std::u8string & str) const
        {
            auto & v = dynamic_cast<property_value_type<T> &>(value);
            str = _fetch(v.get());
        }

        std::shared_ptr<property_value> serialize(const std::u8string & str) const
        {
            auto value = std::make_shared <property_value_type<T>>(_store(str));
            return value;
        }

        std::shared_ptr<property_interpolator> create_interpolator() const
        {
            return std::make_shared<property_interpolator_default<T>>();
        }
    };

    template<typename SetterT, typename GetterT>
    static auto make_accessor(SetterT setter, GetterT getter,
        std::function<std::decay_t<typename member_traits<GetterT>::value_type>(const std::u8string &)> store = property_parser<std::decay_t<typename member_traits<GetterT>::value_type>>,
        std::function<std::u8string(const std::decay_t<typename member_traits<GetterT>::value_type> &)> fetch = nullptr)
    {
        static_assert(std::is_member_function_pointer<SetterT>::value);
        static_assert(std::is_member_pointer<GetterT>::value);

        return std::make_shared<property_accessor_impl<typename member_traits<SetterT>::instance_type, std::decay_t<typename member_traits<GetterT>::value_type>, SetterT, GetterT>>(setter, getter, store, fetch);
    }

    typedef std::map<std::u8string, std::shared_ptr<core::property_accessor>> property_table;

    //---------------------------------------------------------------- interpolator

    template<typename T>
    class property_interpolator_type : public property_interpolator
    {
    public:
        property_interpolator_type() = default;
        property_interpolator_type(std::function<T(const T & s, const T & e, float32_t i)> inter) : _inter(inter) {};
        property_value & start() { return _start; }
        property_value & end() { return _end; }

        void set_values(const T & start, const T & end)
        {
            _start.set(start);
            _end.set(end);
        }

        void set_inter(std::function<T(const T & s, const T & e, float32_t i)> fun)
        {
            _inter = fun;
        }

        void interpolate(object & self, core::property_accessor & accesor, float32_t proportion)
        {
            auto & a = dynamic_cast<property_accessor_type<T> &>(accesor);
            a.set_value(self, _inter(_start.get(), _end.get(), proportion));
        }

        void interpolate(core::property_value & value, float32_t proportion)
        {
            auto & v = dynamic_cast<property_value_type<T> &>(value);
            v.set(_inter(_start.get(), _end.get(), proportion));
        }

    private:
        property_value_type<T> _start;
        property_value_type<T> _end;
        std::function<T(const T & s, const T & e, float32_t p)> _inter;
    };

    template<typename T>
    std::shared_ptr<property_interpolator_type<T>> make_interpolator(std::function<T(const T & s, const T & e, float32_t i)> inter)
    {
        return std::make_shared<property_interpolator_type<T>>(inter);
    }

    // specifize some 
    template<>
    class property_interpolator_default<std::u8string> : public property_interpolator
    {
    public:
        property_interpolator_default() = default;
        property_value & start() { return _start; }
        property_value & end() { return _end; }

        void set_values(const std::u8string & start, const std::u8string & end)
        {
            _start.set(start);
            _end.set(end);
        }

        void interpolate(object & self, core::property_accessor & accesor, float32_t proportion)
        {
            auto & a = dynamic_cast<property_accessor_type<std::u8string> &>(accesor);
            a.set_value(self, proportion < 0.5f ? _start.get() : _end.get());
        }

        void interpolate(core::property_value & value, float32_t proportion)
        {
            auto & v = dynamic_cast<property_value_type<std::u8string> &>(value);
            v.set(proportion < 0.5f ? _start.get() : _end.get());
        }

    private:
        property_value_type<std::u8string> _start;
        property_value_type<std::u8string> _end;
    };


    template<typename T>
    class property_interpolator_keyframes : public core::property_interpolator
    {
        typedef std::pair<float32_t, core::property_value_type<T>> pair_type;
    public:
        property_interpolator_keyframes() = default;
        core::property_value & start() { return _values.front().second; }
        core::property_value & end() { return _values.back().second; }

        void set_values(std::initializer_list<std::pair<float32_t, T>> il)
        {
            std::copy(il.begin(), il.end(), std::back_inserter(_values));
        }

        void interpolate(core::object & self, core::property_accessor & accesor, float32_t proportion)
        {
            auto & a = dynamic_cast<core::property_accessor_type<T> &>(accesor);
            auto iter = std::upper_bound(_values.rbegin(), _values.rend(), proportion, [](float32_t value, const pair_type & pair) { return  pair.first <= value; });
            if (iter != _values.rend())
                a.set_value(self, iter->second.get());
            else
                a.set_value(self, _values.back().second.get());
        }

        void interpolate(core::property_value & value, float32_t proportion)
        {
            auto & v = dynamic_cast<core::property_value_type<T> &>(value);
            auto iter = std::upper_bound(_values.begin(), _values.end(), proportion, [](float32_t value, const pair_type & pair) { return pair.first < value; });
            if (iter != _values.end())
                v.set(iter->second.get());
            else
                v.set(_values.back().second.get());
        }

    private:
        std::vector<pair_type> _values;
    };

    template<>
    inline core::color property_interpolator_calc<core::color>(const core::color & s, const core::color & e, float32_t proportion)
    {
        auto a = s.a + (e.a - s.a) * proportion;
        auto r = s.r + (e.r - s.r) * proportion;
        auto g = s.g + (e.g - s.g) * proportion;
        auto b = s.b + (e.b - s.b) * proportion;
        return core::color(uint8_t(a), uint8_t(r), uint8_t(g), uint8_t(b));
    }


    //------------------------------------------------------------------------------- animation

    enum class animation_state
    {
        waiting = 0,
        running,
        pausing,
    };

    struct animation : public core::object
    {
        virtual ~animation() = default;
        virtual bool update() = 0;

        animation_state state() const { return _state; }
        void setLoop(int32_t loop) { _loop = loop; }
        void setDuration(std::chrono::nanoseconds duration) { _duration = duration; }
        void setCurve(std::function<float32_t(float32_t)> curve) { _curve = curve; }

        void yield() { _time = core::datetime::steady_ns() + _duration * _loop_index; }
        void reset() { _time = core::datetime::steady_ns(); _loop_index = 0; }
        void start() { setState(animation_state::running); }
        void stop() { setState(animation_state::waiting); }
        void pause() { setState(animation_state::pausing); }
        void resume() { setState(animation_state::running); }

        bool waiting() const { return _state == animation_state::waiting; }
        bool running() const { return _state == animation_state::running; }
        bool pausing() const { return _state == animation_state::pausing; }

        void setState(animation_state state)
        {
            check_invoke();
            if (_state == state)
                return;

            auto old_state = _state;
            _state = state;
            switch (state)
            {
            case animation_state::running:
                _time = core::datetime::steady_ns();
                if (old_state == animation_state::waiting)
                    _loop_index = 0;
                started();
                break;
            case animation_state::waiting:
                _time = 0ns;
                _loop_index = 0;
                stoped();
                break;
            case animation_state::pausing:
                paused();
                break;
            }
            changed(old_state, _state);
        }

    protected:
        std::chrono::nanoseconds _time = 0ns;
        std::chrono::nanoseconds _duration = 1s;
        int32_t _loop = 1;
        int32_t _loop_index = 0;
        animation_state _state = animation_state::waiting;
        std::function<float32_t(float32_t)> _curve = [](float32_t val) { return std::clamp(val, 0.0f, 1.0f); };

    public:
        core::event<void(animation_state from, animation_state to)> changed;
        core::event<void()> started;
        core::event<void()> stoped;
        core::event<void()> paused;
        core::event<void(int32_t)> looped;
    };

    struct property_animation : public animation
    {
        property_animation(std::shared_ptr<core::object> object, std::shared_ptr<core::property_accessor> accessor, std::shared_ptr<core::property_interpolator> interpolator) :_object(object), _accessor(accessor), _interpolator(interpolator)
        {

        }

        bool update();

        std::weak_ptr<core::object> _object;
        std::shared_ptr<core::property_accessor> _accessor;
        std::shared_ptr<core::property_interpolator> _interpolator;
    };



    template<>
    inline float32_t property_parser<float32_t>(const std::u8string & str)
    {
        return std::atof(reinterpret_cast<const char *>(str.c_str()));
    }

    template<>
    inline core::vec2<float32_t> property_parser<core::vec2<float32_t>>(const std::u8string & str)
    {
        std::vector<std::u8string> strs = core::split(str, u8' ');
        if (strs.size() == 1)
            return core::vec2<float32_t>{ property_parser<float32_t>(strs[0]) };
        if (strs.size() == 2)
            return { property_parser<float32_t>(strs[0]), property_parser<float32_t>(strs[1]) };
        return {};
    }

    template<> std::u8string property_parser<std::u8string>(const std::u8string & str);
    template<> bool property_parser<bool>(const std::u8string & str);
    template<> core::color property_parser<core::color>(const std::u8string & str);
    template<> core::vec2<core::color> property_parser<core::vec2<core::color>>(const std::u8string & str);
    template<> core::vec4<core::color> property_parser<core::vec4<core::color>>(const std::u8string & str);

    template<> core::dimenf property_parser<core::dimenf>(const std::u8string & str);
    template<> core::vec2<core::dimenf> property_parser<core::vec2<core::dimenf>>(const std::u8string & str);
    template<> core::vec4<core::dimenf> property_parser<core::vec4<core::dimenf>>(const std::u8string & str);

    template<> std::chrono::nanoseconds property_parser<std::chrono::nanoseconds>(const std::u8string & str);
}
