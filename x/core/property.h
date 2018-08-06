#pragma once

namespace core
{
    struct property_instance
    {
        virtual ~property_instance() = default;
        virtual void apply(object & self) = 0;
    };

    struct property_accessor
    {
        virtual ~property_accessor() = default;
    };

    template<typename T>
    struct property_accessor_type : public property_accessor
    {
        virtual void store(object & self_, const T & val) = 0;
        virtual T fetch(object & self_) = 0;
    };


    template<typename ClassT, typename T, typename SetterT, typename GetterT>
    struct property_accessor_impl : public property_accessor_type<T>
    {
        property_accessor_impl(SetterT setter_, GetterT getter_) : setter(setter_), getter(getter_) {}

        SetterT setter;
        GetterT getter;

        void store(object & self_, const T & val_)
        {
            auto & self = dynamic_cast<typename member_function_traits<SetterT>::instance_type &>(self_);
            (self.*setter)(val_);
        }

        T fetch(object & self_)
        {
            auto & self = dynamic_cast<const typename member_function_traits<SetterT>::instance_type &>(self_);
            return (self.*getter)();
        }
    };

    template<typename SetterT, typename GetterT>
    static auto make_accessor(SetterT setter_, GetterT getter_)
    {
        static_assert(std::is_member_function_pointer<SetterT>::value);
        static_assert(std::is_member_function_pointer<GetterT>::value);
        return std::make_shared<property_accessor_impl<typename member_function_traits<SetterT>::instance_type, std::decay_t<typename member_function_traits<GetterT>::return_type>, SetterT, GetterT>>(setter_, getter_);
    }


    struct property_serializer
    {
        virtual ~property_serializer() = default;
        virtual void store(std::string & str, object & self_, std::shared_ptr<property_accessor> accesor) = 0;
        virtual void fetch(const std::string & str, object & self_, std::shared_ptr<property_accessor> accesor) = 0;
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

        void store(std::string & str, object & self_, std::shared_ptr<property_accessor> accesor)
        {
            auto accessor_ = std::dynamic_pointer_cast<property_accessor_type<T>>(accesor);
            if(accessor_)
                str = _fun_fetch(accessor_->fetch(self_));
        }

        void fetch(const std::string & str, object & self_, std::shared_ptr<property_accessor> accesor)
        {
            auto accessor_ = std::dynamic_pointer_cast<property_accessor_type<T>>(accesor);
            if (accessor_)
                accessor_->store(self_, _fun_store(str));
        }

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
