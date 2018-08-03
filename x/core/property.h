#pragma once

namespace core
{
    struct property_asccesor
    {
        virtual ~property_asccesor() = default;
        virtual void apply(object & self) = 0;
    };

    struct property_builder
    {
        std::string name;

        property_builder(std::string name_) : name(name_) {}
        virtual ~property_builder() = default;
        virtual std::shared_ptr<property_asccesor> build(const std::string_view & str) = 0;
    };


    template<typename ClassT, typename ValT, typename SetterT, typename GetterT>
    struct property_asccesor_instance : public property_asccesor
    {
        SetterT setter;
        GetterT getter;
        ValT val;

        void apply(object & self_)
        {
            auto & self = dynamic_cast<typename member_function_traits<SetterT>::instance_type &>(self_);
            (self.*setter)(val);
        }
    };

    template<typename ClassT, typename ValT, typename SetterT, typename GetterT>
    struct property_builder_instance : public property_builder
    {
        property_builder_instance(std::string name_,
            SetterT setter_,
            GetterT getter_,
            std::function<ValT(const std::string_view &)> parser_) : property_builder(name_), setter(setter_), getter(getter_), parser(parser_) {}

        SetterT setter;
        GetterT getter;
        std::function<ValT(const std::string_view &)> parser;

        std::shared_ptr<property_asccesor> build(const std::string_view & str)
        {
            auto asccesor = std::make_shared<property_asccesor_instance<ClassT, ValT, SetterT, GetterT>>();
            asccesor->setter = setter;
            asccesor->getter = getter;
            asccesor->val = parser(str);
            return asccesor;
        }
    };


    template<typename SetterT, typename GetterT>
    static auto make_builder(std::string name_,
        SetterT setter_,
        GetterT getter_,
        std::function<std::decay_t<typename member_function_traits<GetterT>::return_type>(const std::string_view &)> parser_)
    {
        static_assert(std::is_member_function_pointer<SetterT>::value);
        static_assert(std::is_member_function_pointer<GetterT>::value);
        return std::make_shared<property_builder_instance<typename member_function_traits<SetterT>::instance_type, std::decay_t<typename member_function_traits<GetterT>::return_type>, SetterT, GetterT>>(name_, setter_, getter_, parser_);
    }

    std::string parseString(const std::string_view & str);
    core::color32 parseColor(const std::string_view & str);
    core::vec2<core::color32> parseColor2D(const std::string_view & str);
    core::vec4<core::color32> parseColor4D(const std::string_view & str);
    core::dimensionf parseDimension(const std::string_view & str);
    core::vec2<core::dimensionf> parseDimension2D(const std::string_view & str);
    core::vec4<core::dimensionf> parseDimension4D(const std::string_view & str);
}
