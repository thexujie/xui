#pragma once

namespace core
{
    template<typename Enum>
    struct bitflag
    {
        bitflag() = default;
        bitflag(Enum e) : _e(e) {}

        using underlying = typename std::underlying_type<Enum>::type;

        void set(Enum e, bool b)
        {

        }

        bitflag operator | (Enum e) const
        {
            return static_cast<Enum> (static_cast<underlying>(_e) | static_cast<underlying>(e));
        }

        bitflag operator & (Enum e) const
        {
            return static_cast<Enum> (static_cast<underlying>(_e) & static_cast<underlying>(e));
        }

        bitflag operator !() const
        {
            return static_cast<Enum> (!static_cast<underlying>(_e));
        }

        bitflag operator ~() const
        {
            return static_cast<Enum> (~static_cast<underlying>(_e));
        }


        bitflag & operator |= (Enum e)
        {
            return static_cast<Enum> (static_cast<underlying>(_e) | static_cast<underlying>(e));
        }

        bitflag & operator &= (Enum e)
        {
            return static_cast<Enum> (static_cast<underlying>(_e) & static_cast<underlying>(e));
        }

        operator underlying() const
        {
            return static_cast<underlying>(_e);
        }

    private:

        Enum _e;
    };
}

template<typename Enum>
struct enable_bitmasks
{
    static const bool enable = false;
};

template<typename Enum>
typename std::enable_if<enable_bitmasks<Enum>::enable, Enum>::type operator |(Enum lhs, Enum rhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum> (static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}

template<typename Enum>
typename std::enable_if<enable_bitmasks<Enum>::enable, Enum>::type operator &(Enum lhs, Enum rhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum> (static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}

template<typename Enum>
typename std::enable_if<enable_bitmasks<Enum>::enable, Enum>::type & operator |=(Enum & lhs, Enum rhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    return lhs = static_cast<Enum> (static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}

template<typename Enum>
typename std::enable_if<enable_bitmasks<Enum>::enable, Enum>::type & operator &=(Enum & lhs, Enum rhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    return lhs = static_cast<Enum> (static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}

template<typename Enum>
typename std::enable_if<enable_bitmasks<Enum>::enable, Enum>::type operator !(Enum & lhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    return lhs = static_cast<Enum> (!static_cast<underlying>(lhs));
}

template<typename Enum>
typename std::enable_if<enable_bitmasks<Enum>::enable, Enum>::type operator ~(Enum & lhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    return lhs = static_cast<Enum> (~static_cast<underlying>(lhs));
}
