#pragma once

namespace core
{
    class property_value
    {
    public:
        virtual ~property_value() = default;
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

    struct property_accessor
    {
        virtual ~property_accessor() = default;
        virtual void set(object & self, const property_value & val) = 0;
        virtual void get(object & self, property_value & val) = 0;
    };

    template<typename T>
    struct property_accessor_type : public property_accessor
    {
        virtual void set_value(object & self, const T & val) = 0;
        virtual T get_value(object & self) = 0;
    };


    template<typename ClassT, typename T, typename SetterT, typename GetterT>
    struct property_accessor_impl : public property_accessor_type<T>
    {
        property_accessor_impl(SetterT setter_, GetterT getter_) : setter(setter_), getter(getter_) {}

        SetterT setter;
        GetterT getter;

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
    };

    template<typename SetterT, typename GetterT>
    static auto make_accessor(SetterT setter_, GetterT getter_)
    {
        static_assert(std::is_member_function_pointer<SetterT>::value);
        static_assert(std::is_member_function_pointer<GetterT>::value);
        return std::make_shared<property_accessor_impl<typename member_function_traits<SetterT>::instance_type, std::decay_t<typename member_function_traits<GetterT>::return_type>, SetterT, GetterT>>(setter_, getter_);
    }


    //----------------------------------------------------------------------------------------------------
    struct property_serializer
    {
        virtual ~property_serializer() = default;
        virtual void get(std::string & str, object & self_, property_accessor & accesor) = 0;
        virtual void set(const std::string & str, object & self_, property_accessor & accesor) = 0;
        virtual void get(std::string & str, property_value & value) = 0;
        virtual void set(const std::string & str, property_value & value) = 0;

        virtual std::shared_ptr<property_value> serialize(const std::string & str) = 0;
    };

    template<typename T>
    struct property_serializer_type : public property_serializer
    {
    };

    template<typename T>
    struct property_serializer_string_impl : public property_serializer_type<T>
    {
        property_serializer_string_impl(){}

        property_serializer_string_impl(std::function<T(const std::string &)> store, std::function<std::string(const T &)> fetch)
        {
            _fun_store = store;
            _fun_fetch = fetch;
        }

        void get(std::string & str, object & self_, property_accessor & accesor)
        {
            auto & a = dynamic_cast<property_accessor_type<T> &>(accesor);
            str = _fun_fetch(a.get_value(self_));
        }

        void set(const std::string & str, object & self_, property_accessor & accesor)
        {
            auto & a = dynamic_cast<property_accessor_type<T> &>(accesor);
            a.set_value(self_, _fun_store(str));
        }

        void get(std::string & str, property_value & value)
        {
            auto & v = dynamic_cast<property_value_type<T> &>(value);
            str = _fun_fetch(v.get());
        }

        void set(const std::string & str, property_value & value)
        {
            auto & v = dynamic_cast<property_value_type<T> &>(value);
            v.set(_fun_store(str));
        }

        std::shared_ptr<property_value> serialize(const std::string & str)
        {
            auto value = std::make_shared <property_value_type<T>>(_fun_store(str));
            return value;
        }

    private:
        std::function<T(const std::string &)> _fun_store;
        std::function<std::string(const T &)> _fun_fetch;
    };

    template<typename T>
    auto make_serializer(std::function<T(const std::string &)> store, std::function<std::string(const T &)> fetch)
    {
        return std::make_shared<property_serializer_string_impl<T>>(store, fetch);
    }

    typedef std::pair<std::shared_ptr<core::property_accessor>, std::shared_ptr<core::property_serializer>> property_pair;
    typedef std::map<std::string, property_pair> property_table;


    template<typename SetterT, typename GetterT>
    static auto make_property(SetterT setter_, GetterT getter_,
        std::function<std::decay_t<typename core::member_function_traits<GetterT>::return_type>(const std::string &)> parser_)
    {
        typedef std::decay_t<typename core::member_function_traits<GetterT>::return_type> ValT;
        static_assert(std::is_member_function_pointer<SetterT>::value);
        static_assert(std::is_member_function_pointer<GetterT>::value);
        return std::make_pair(core::make_accessor(setter_, getter_), core::make_serializer<ValT>(parser_, nullptr));
    }

    std::string parseString(const std::string & str);
    core::color32 parseColor(const std::string & str);
    core::vec2<core::color32> parseColor2D(const std::string & str);
    core::vec4<core::color32> parseColor4D(const std::string & str);
    core::dimensionf parseDimension(const std::string & str);
    core::vec2<core::dimensionf> parseDimension2D(const std::string & str);
    core::vec4<core::dimensionf> parseDimension4D(const std::string & str);
}
