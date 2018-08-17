#pragma once

namespace core
{
    class property_value
    {
    public:
        virtual ~property_value() = default;
    };

    struct property_accessor
    {
        virtual ~property_accessor() = default;
        virtual void set(object & self, const property_value & val) = 0;
        virtual void get(object & self, property_value & val) = 0;

        virtual void set(const std::string & str, object & self) = 0;
        virtual void get(std::string & str, object & self) = 0;

        virtual void serialize(const std::string & str, property_value & value) const = 0;
        virtual void serialize(property_value & value, std::string & str) const = 0;

        virtual std::shared_ptr<property_value> serialize(const std::string & str) const = 0;
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

    private:
        T _val;
    };

    template<typename T>
    struct property_accessor_type : public property_accessor
    {
        virtual void set_value(object & self, const T & val) = 0;
        virtual T get_value(object & self) = 0;
    };

    template<typename T>
    T property_interpolator_calc(const T & s, const T & e, float32_t proportion)
    {
        return s + static_cast<T>((e - s) * proportion);
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
            std::function<T(const std::string &)> store, std::function<std::string(const T &)> fetch) : setter(setter_), getter(getter_), _store(store), _fetch(fetch) {}

        SetterT setter;
        GetterT getter;
        std::function<T(const std::string &)> _store;
        std::function<std::string(const T &)> _fetch;

        void set(object & self, const property_value & val)
        {
            auto & s = dynamic_cast<typename member_function_traits<SetterT>::instance_type &>(self);
            auto & v = dynamic_cast<const property_value_type<T> &>(val);
            (s.*setter)(v.get());
        }

        void get(object & self, property_value & val)
        {
            auto & s = dynamic_cast<const typename member_function_traits<SetterT>::instance_type &>(self);
            auto & v = dynamic_cast<property_value_type<T> &>(val);
            v.set((s.*getter)());
        }

        void set_value(object & self, const T & val)
        {
            auto & s = dynamic_cast<typename member_function_traits<SetterT>::instance_type &>(self);
            (s.*setter)(val);
        }

        T get_value(object & self)
        {
            auto & s = dynamic_cast<typename member_function_traits<SetterT>::instance_type &>(self);
            return (s.*getter)();
        }


        void get(std::string & str, object & self)
        {
            str = _fetch(get_value(self));
        }

        void set(const std::string & str, object & self)
        {
            set_value(self, _store(str));
        }

        void serialize(const std::string & str, property_value & value) const
        {
            auto & v = dynamic_cast<property_value_type<T> &>(value);
            v.set(_store(str));
        }

        void serialize(property_value & value, std::string & str) const
        {
            auto & v = dynamic_cast<property_value_type<T> &>(value);
            str = _fetch(v.get());
        }

        std::shared_ptr<property_value> serialize(const std::string & str) const
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
        std::function<std::decay_t<typename member_function_traits<GetterT>::return_type>(const std::string &)> store,
        std::function<std::string(const std::decay_t<typename member_function_traits<GetterT>::return_type> &)> fetch)
    {
        static_assert(std::is_member_function_pointer<SetterT>::value);
        static_assert(std::is_member_function_pointer<GetterT>::value);
        return std::make_shared<property_accessor_impl<typename member_function_traits<SetterT>::instance_type, std::decay_t<typename member_function_traits<GetterT>::return_type>, SetterT, GetterT>>(setter, getter, store, fetch);
    }

    typedef std::map<std::string, std::shared_ptr<core::property_accessor>> property_table;

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


    std::string parseString(const std::string & str);
    core::color32 parseColor(const std::string & str);
    core::vec2<core::color32> parseColor2D(const std::string & str);
    core::vec4<core::color32> parseColor4D(const std::string & str);
    core::dimensionf parseDimension(const std::string & str);
    core::vec2<core::dimensionf> parseDimension2D(const std::string & str);
    core::vec4<core::dimensionf> parseDimension4D(const std::string & str);
    std::chrono::nanoseconds parseDuration(const std::string & str);


    // specifize some 
    template<>
    class property_interpolator_default<std::string> : public property_interpolator
    {
    public:
        property_interpolator_default() = default;
        property_value & start() { return _start; }
        property_value & end() { return _end; }

        void set_values(const std::string & start, const std::string & end)
        {
            _start.set(start);
            _end.set(end);
        }

        void interpolate(object & self, core::property_accessor & accesor, float32_t proportion)
        {
            auto & a = dynamic_cast<property_accessor_type<std::string> &>(accesor);
            a.set_value(self, proportion < 0.5f ? _start.get() : _end.get());
        }

        void interpolate(core::property_value & value, float32_t proportion)
        {
            auto & v = dynamic_cast<property_value_type<std::string> &>(value);
            v.set(proportion < 0.5f ? _start.get() : _end.get());
        }

    private:
        property_value_type<std::string> _start;
        property_value_type<std::string> _end;
    };

    template<>
    inline core::color32 property_interpolator_calc<core::color32>(const core::color32 & s, const core::color32 & e, float32_t proportion)
    {
        auto a = s.a + (e.a - s.a) * proportion;
        auto r = s.r + (e.r - s.r) * proportion;
        auto g = s.g + (e.g - s.g) * proportion;
        auto b = s.b + (e.b - s.b) * proportion;
        return core::color32(a, r, g, b);
    }


    //------------------------------------------------------------------------------- animation

    enum class animation_state
    {
        waiting = 0,
        running,
        pausing,
    };

    struct animation
    {
        virtual ~animation() = default;
        virtual void update() = 0;

        animation_state state() const { return _state; }
        bool waiting() const { return _state == animation_state::waiting; }
        bool running() const { return _state == animation_state::running; }
        bool pausing() const { return _state == animation_state::pausing; }

        void setLoop(int32_t loop) { _loop = loop; }
        void setDuration(std::chrono::nanoseconds duration) { _duration = duration; }
        void setCurve(std::function<float32_t(float32_t)> curve) { _curve = curve; }

        void start() { _state = animation_state::running; _time = core::datetime::steady(); _loop_index = 0; }
        void stop() { _state = animation_state::waiting; _time = 0ns; }
        void pause() { _state = animation_state::pausing; }
        void resume() { _state = animation_state::running;  _time = core::datetime::steady(); }

    protected:
        std::chrono::nanoseconds _time = 0ns;
        std::chrono::nanoseconds _duration = 1s;
        int32_t _loop = 1;
        int32_t _loop_index = 0;
        animation_state _state = animation_state::waiting;
        std::function<float32_t(float32_t)> _curve = [](float32_t val) { return std::clamp(val, 0.0f, 1.0f); };

    public:
        core::event<void()> stoped;
        core::event<void(int32_t)> looped;
    };

    struct property_animation : public animation
    {
        property_animation(std::shared_ptr<core::object> object, std::shared_ptr<core::property_accessor> accessor, std::shared_ptr<core::property_interpolator> interpolator) :_object(object), _accessor(accessor), _interpolator(interpolator)
        {

        }

        void update();

        std::weak_ptr<core::object> _object;
        std::shared_ptr<core::property_accessor> _accessor;
        std::shared_ptr<core::property_interpolator> _interpolator;
    };
}
