#pragma once

namespace core
{
    template<typename ValT>
    class vec2
    {
    public:
        vec2() : x(), y() { }

        vec2(ValT _x, ValT _y) : x(_x), y(_y) { }

        vec2(const vec2 & vec) : x(vec.x), y(vec.y) { }

        void set(ValT _x, ValT _y)
        {
            x = _x;
            y = _y;
        }

        void expand(ValT _cx, ValT _cy)
        {
            w += _cx * 2;
            h += _cy * 2;
        }

        void set_empty()
        {
            w = 0;
            h = 0;
        }

        bool empty() const
        {
            if (std::is_floating_point<ValT>::value)
                return std::abs(w) < std::numeric_limits<ValT>::epsilon() || std::abs(h) < std::numeric_limits<ValT>::epsilon();
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
        vec2 operator +(const vec2 & vec) const
        {
            return vec2(x + vec.x, y + vec.y);
        }

        vec2 operator -(const vec2 & vec) const
        {
            return vec2(x - vec.x, y - vec.y);
        }

        vec2 operator *(const vec2 & vec) const
        {
            return vec2(x * vec.x, y * vec.y);
        }

        vec2 operator /(const vec2 & vec) const
        {
            return vec2(x / vec.x, y / vec.y);
        }

        vec2 & operator +=(const vec2 & vec)
        {
            x += vec.x;
            y += vec.y;
            return *this;
        }

        vec2 & operator -=(const vec2 & vec)
        {
            x -= vec.x;
            y -= vec.y;
            return *this;
        }

        vec2 & operator *=(const vec2 & vec)
        {
            x *= vec.x;
            y *= vec.y;
            return *this;
        }

        vec2 & operator /=(const vec2 & vec)
        {
            x /= vec.x;
            y /= vec.y;
            return *this;
        }

        vec2 operator *(const ValT & rate) const
        {
            return vec2(x * rate, y * rate);
        }

        vec2 operator /(const ValT & rate)
        {
            return vec2(x / rate, y / rate);
        }

        vec2 & operator *=(const ValT & rate)
        {
            x *= rate;
            y *= rate;
            return *this;
        }

        vec2 & operator /=(const ValT & rate)
        {
            x /= rate;
            y /= rate;
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
            if (std::is_floating_point<ValT>::value)
                return std::fabs(x - vec.x) < std::numeric_limits<ValT>::epsilon() &&
                std::fabs(y - vec.y) < std::numeric_limits<ValT>::epsilon();
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

        template<typename = std::enable_if_t<ValT, int32_t>::value>
        vec2<float32_t> to_rc32f() const
        {
            return { (float32_t)x, (float32_t)y };
        }

        template<typename T2>
        vec2<T2> to() const
        {
            return { (T2)x, (T2)y };
        }

    public:
        union
        {
            struct
            {
                //! 横坐标。
                ValT x;
                //! 纵坐标。
                ValT y;
            };

            struct
            {
                //! 横坐标。
                ValT left;
                //! 纵坐标。
                ValT top;
            };

            struct
            {
                //! 宽。
                ValT cx;
                //! 高。
                ValT cy;
            };

            struct
            {
                //! 宽。
                ValT w;
                //! 高。
                ValT h;
            };

            struct
            {
                //! 列。
                ValT col;
                //! 行。
                ValT row;
            };
        };
    };

    typedef vec2<int32_t> si32i;
    typedef vec2<float32_t> si32f;
    typedef vec2<float32_t> vec2f;
    typedef vec2<float64_t> si64f;

    typedef vec2<int32_t> pt32i;
    typedef vec2<float32_t> pt32f;
    typedef vec2<float64_t> pt64f;
}
