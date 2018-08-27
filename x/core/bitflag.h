#pragma once

namespace core
{
    template<typename Enum>
    struct bitflag
    {
        bitflag() = default;
        bitflag(Enum e) : _e(e) {}
        bitflag(std::nullptr_t) : _e(static_cast<Enum>(0)) {}

        using underlying = typename std::underlying_type<Enum>::type;

        bitflag & operator = (Enum e)
        {
            _e = e;
            return *this;
        }

        bitflag & set(Enum e, bool b)
        {
            if(b)
                _e = static_cast<Enum> (static_cast<underlying>(_e) | static_cast<underlying>(e));
            else
                _e = static_cast<Enum> (static_cast<underlying>(_e) & (~static_cast<underlying>(e)));
            return *this;
        }

        bool any(Enum e) const
        {
            return (static_cast<underlying>(_e) & static_cast<underlying>(e)) != 0;
        }

        bool all(Enum e) const
        {
            return (static_cast<underlying>(_e) & static_cast<underlying>(e)) == static_cast<underlying>(e);
        }

        bool any(bitflag bf) const
        {
            return (static_cast<underlying>(_e) & static_cast<underlying>(bf._e)) != 0;
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
            _e = static_cast<Enum> (static_cast<underlying>(_e) | static_cast<underlying>(e));
            return *this;
        }

        bitflag & operator &= (Enum e)
        {
            _e = static_cast<Enum> (static_cast<underlying>(_e) & static_cast<underlying>(e));
            return *this;
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
typename std::enable_if<enable_bitmasks<Enum>::enable, Enum>::type operator |(const Enum & lhs, const Enum & rhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum> (static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}

template<typename Enum>
typename std::enable_if<enable_bitmasks<Enum>::enable, Enum>::type operator &(const Enum & lhs, const Enum & rhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum> (static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}

template<typename Enum>
typename std::enable_if<enable_bitmasks<Enum>::enable, Enum>::type & operator |=(Enum & lhs, const Enum & rhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    return lhs = static_cast<Enum> (static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}

template<typename Enum>
typename std::enable_if<enable_bitmasks<Enum>::enable, Enum>::type & operator &=(Enum & lhs, const Enum & rhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    return lhs = static_cast<Enum> (static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}

template<typename Enum>
std::enable_if_t<enable_bitmasks<Enum>::enable, bool> operator !(const Enum & lhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<underlying>(lhs) == 0;
}

template<typename Enum>
typename std::enable_if<enable_bitmasks<Enum>::enable, Enum>::type operator ~(const Enum & lhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum> (~static_cast<underlying>(lhs));
}
