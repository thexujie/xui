#pragma once

namespace core
{
    template<typename>
    struct member_function_traits;

    template<typename Return, typename Object, typename... Args>
    struct member_function_traits<Return(Object::*)(Args ...)>
    {
        typedef Return return_type;
        typedef Object instance_type;
        typedef Object & instance_reference;

        static constexpr size_t argument_count = sizeof...(Args);
    };

    template<typename Return, typename Object, typename... Args>
    struct member_function_traits<Return(Object::*)(Args ...) const>
    {
        typedef Return return_type;
        typedef Object instance_type;
        typedef Object const & instance_reference;

        // Can mess with Args... if you need to, for example:
        static constexpr size_t argument_count = sizeof...(Args);
    };
}
