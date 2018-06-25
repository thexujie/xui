#pragma once


namespace core::math
{
    template<typename ValT>
    class vec2
    {
    public:
        vec2() : x(0), y(0) { }

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

        bool is_empty() const { return w == 0 || h == 0; }

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
            return x == vec.x && y == vec.y;
        }

        bool operator !=(const vec2 & vec) const
        {
            return x != vec.x || y != vec.y;
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

        template<typename ValT2>
        vec2<ValT2> to() const
        {
            return { (ValT2)x, (ValT2)y };
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
    typedef vec2<float64_t> si64f;

    typedef vec2<int32_t> pt32i;
    typedef vec2<float32_t> pt32f;
    typedef vec2<float64_t> pt64f;
}
