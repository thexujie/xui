#pragma once

namespace core
{
    template<typename T>
    class vec2
    {
    public:
        vec2() : x(), y() { }

        explicit vec2(T _t) : x(_t), y(_t) { }

        vec2(T _x, T _y) : x(_x), y(_y) { }

        vec2(const vec2 & vec) : x(vec.x), y(vec.y) { }

        void set(T _x, T _y)
        {
            x = _x;
            y = _y;
        }

        void expand(T _cx, T _cy)
        {
            w += _cx * 2;
            h += _cy * 2;
        }

        void clear()
        {
            w = 0;
            h = 0;
        }

        bool empty() const
        {
            if (std::is_floating_point<T>::value)
                return std::isnan(w) || std::isnan(h) || std::abs(w) <= std::numeric_limits<T>::epsilon() || std::abs(h) <= std::numeric_limits<T>::epsilon();
            else
                return w == 0 || h == 0;
        }

        vec2 & operator =(const vec2 & vec)
        {
            x = vec.x;
            y = vec.y;
            return *this;
        }

        //------------------------------------------------------- 四则运算
        template<typename = decltype(std::declval<T>() + std::declval<T>())>
        vec2 operator +(const vec2 & vec) const
        {
            return vec2(x + vec.x, y + vec.y);
        }

        template<typename = decltype(std::declval<T>() - std::declval<T>())>
        vec2 operator -(const vec2 & vec) const
        {
            return vec2(x - vec.x, y - vec.y);
        }

        template<typename = decltype(std::declval<T>() * std::declval<T>())>
        vec2 operator *(const vec2 & vec) const
        {
            return vec2(x * vec.x, y * vec.y);
        }

        template<typename = decltype(std::declval<T>() / std::declval<T>())>
        vec2 operator /(const vec2 & vec) const
        {
            return vec2(x / vec.x, y / vec.y);
        }

        template<typename = decltype(std::declval<T>() += std::declval<T>())>
        vec2 & operator +=(const vec2 & vec)
        {
            x += vec.x;
            y += vec.y;
            return *this;
        }

        template<typename = decltype(std::declval<T>() -= std::declval<T>())>
        vec2 & operator -=(const vec2 & vec)
        {
            x -= vec.x;
            y -= vec.y;
            return *this;
        }

        template<typename = decltype(std::declval<T>() *= std::declval<T>())>
        vec2 & operator *=(const vec2 & vec)
        {
            x *= vec.x;
            y *= vec.y;
            return *this;
        }

        template<typename = decltype(std::declval<T>() /= std::declval<T>())>
        vec2 & operator /=(const vec2 & vec)
        {
            x /= vec.x;
            y /= vec.y;
            return *this;
        }

        template<typename = decltype(std::declval<T>() * std::declval<T>())>
        vec2 operator * (const T & rate) const
        {
            return vec2(x * rate, y * rate);
        }

        template<typename = std::enable_if_t<!std::is_same_v<T, float32_t>>>
        vec2 operator * (float32_t rate) const
        {
            return vec2(static_cast<T>(x * rate), static_cast<T>(y * rate));
        }

        vec2 operator / (const T & rate)
        {
            return vec2(x / rate, y / rate);
        }

        template<typename = std::enable_if_t<!std::is_same_v<T, float32_t>>>
        vec2 operator / (float32_t rate) const
        {
            return vec2(static_cast<T>(x / rate), static_cast<T>(y / rate));
        }

        vec2 & operator *=(const T & rate)
        {
            x *= rate;
            y *= rate;
            return *this;
        }

        template<typename = std::enable_if_t<!std::is_same_v<T, float32_t>>>
        vec2 & operator *= (float32_t rate)
        {
            x = static_cast<T>(x * rate);
            y = static_cast<T>(y * rate);
            return *this;
        }

        vec2 & operator /=(const T & rate)
        {
            x /= rate;
            y /= rate;
            return *this;
        }

        template<typename = std::enable_if_t<!std::is_same_v<T, float32_t>>>
        vec2 & operator /=(float32_t rate)
        {
            x = static_cast<T>(x / rate);
            y = static_cast<T>(y / rate);
            return *this;
        }

        vec2 operator -() const
        {
            return vec2(-x, -y);
        }

        vec2 operator +() const
        {
            return *this;
        }

        //--------------------------------------------------比较运算
        bool operator ==(const vec2 & vec) const
        {
            if constexpr (std::is_floating_point_v<T>)
                return core::equal(x, vec.x) && core::equal(y, vec.y);
            else
                return x == vec.x && y == vec.y;
        }

        bool operator !=(const vec2 & vec) const
        {
            return !operator ==(vec);
        }

        bool operator <(const vec2 & vec) const
        {
            return x < vec.x && y < vec.y;
        }

        bool operator >(const vec2 & vec) const
        {
            return x > vec.x && y > vec.y;
        }

        bool operator <=(const vec2 & vec) const
        {
            return x <= vec.x && y <= vec.y;
        }

        bool operator >=(const vec2 & vec) const
        {
            return x >= vec.x && y >= vec.y;
        }

        template<typename T2>
        vec2<T2> to() const
        {
            return { static_cast<T2>(x), static_cast<T2>(y) };
        }

        template<class ConvertT>
        auto convert(ConvertT convert) const
        {
            return vec2(convert(x), convert(y));
        }

    public:
        union
        {
            struct
            {
                //! 横坐标。
                T x;
                //! 纵坐标。
                T y;
            };

            struct
            {
                //! 横坐标。
                T left;
                //! 纵坐标。
                T top;
            };

            struct
            {
                //! 宽。
                T cx;
                //! 高。
                T cy;
            };

            struct
            {
                //! 宽。
                T w;
                //! 高。
                T h;
            };

            struct
            {
                //! 列。
                T col;
                //! 行。
                T row;
            };
        };
    };

    typedef vec2<int32_t> si32i;
    typedef vec2<float32_t> si32f;
    typedef vec2<int32_t> vec2i;
    typedef vec2<float32_t> vec2f;
    typedef vec2<float64_t> si64f;

    typedef vec2<int32_t> pt32i;
    typedef vec2<float32_t> pt32f;
    typedef vec2<float64_t> pt64f;
}
