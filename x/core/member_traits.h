#pragma once

namespace core
{
    template<typename>
    struct member_traits;

    template<typename Return, typename Object, typename... Args>
    struct member_traits<Return(Object::*)(Args ...)>
    {
        typedef Return value_type;
        typedef Object instance_type;
        typedef Object & instance_reference;

        static constexpr size_t argument_count = sizeof...(Args);
    };

    template<typename Return, typename Object, typename... Args>
    struct member_traits<Return(Object::*)(Args ...) const>
    {
        typedef Return value_type;
        typedef Object instance_type;
        typedef Object const & instance_reference;

        static constexpr size_t argument_count = sizeof...(Args);
    };

    template<typename Return, typename Object>
    struct member_traits<Return(Object::*)>
    {
        typedef Return value_type;
        typedef Object instance_type;
        typedef Object & instance_reference;
    };
}
