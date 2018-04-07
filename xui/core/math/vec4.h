#pragma once

namespace core { namespace math
{
    template <typename ValT>
    class vec4
    {
    public:
        vec4() : x(0), y(0), z(0), w(0)
        {
        }

        vec4(ValT _x, ValT _y, ValT _z, ValT _w) : x(_x), y(_y), z(_z), w(_w)
        {
        }

        vec4(const vec4 & vec) : x(vec.x), y(vec.y), z(vec.z), w(vec.w)
        {
        }

        void set(ValT _x, ValT _y, ValT _z, ValT _w)
        {
            x = _x;
            y = _y;
            z = _z;
            w = _w;
        }

        vec4 & operator =(const vec4 & vec)
        {
            x = vec.x;
            y = vec.y;
            z = vec.z;
            w = vec.w;
            return *this;
        }

        //------------------------------------------------------- 四则运算
        vec4 operator +(const vec4 & vec) const
        {
            return vec4(x + vec.x, y + vec.y, z + vec.z, w + vec.w);
        }

        vec4 operator -(const vec4 & vec) const
        {
            return vec4(x - vec.x, y - vec.y, z - vec.z, w - vec.w);
        }

        vec4 operator *(const vec4 & vec) const
        {
            return vec4(x * vec.x, y * vec.y, z * vec.z, w * vec.w);
        }

        vec4 operator /(const vec4 & vec) const
        {
            return vec4(x / vec.x, y / vec.y, z / vec.z, w / vec.w);
        }

        vec4 & operator +=(const vec4 & vec)
        {
            x += vec.x;
            y += vec.y;
            z += vec.z;
            w += vec.w;
            return *this;
        }

        vec4 & operator -=(const vec4 & vec)
        {
            x -= vec.x;
            y -= vec.y;
            z -= vec.z;
            w -= vec.w;
            return *this;
        }

        vec4 & operator *=(const vec4 & vec)
        {
            x *= vec.x;
            y *= vec.y;
            z *= vec.z;
            w *= vec.w;
            return *this;
        }

        vec4 & operator /=(const vec4 & vec)
        {
            x /= vec.x;
            y /= vec.y;
            z /= vec.z;
            w /= vec.w;
            return *this;
        }

        vec4 operator *(const ValT & rate) const
        {
            return vec4(x * rate, y * rate, z * rate, w * rate);
        }

        vec4 operator /(const ValT & rate)
        {
            return vec4(x / rate, y / rate, z / rate, w / rate);
        }

        vec4 & operator *=(const ValT & rate)
        {
            x *= rate;
            y *= rate;
            z *= rate;
            w *= rate;
            return *this;
        }

        vec4 & operator /=(const ValT & rate)
        {
            x /= rate;
            y /= rate;
            z /= rate;
            w /= rate;
            return *this;
        }

        vec4 operator -() const
        {
            return vec4(-x, -y, -z, -w);
        }

        vec4 operator +() const
        {
            return *this;
        }

        //--------------------------------------------------比较运算
        bool operator ==(const vec4 & vec) const
        {
            return x == vec.x && y == vec.y && z == vec.z && w == vec.w;
        }

        bool operator !=(const vec4 & vec) const
        {
            return x != vec.x || y != vec.y || z != vec.z || w != vec.w;
        }

        bool operator <(const vec4 & vec) const
        {
            return x < vec.x && y < vec.y && z < vec.z && w < vec.w;
        }

        bool operator >(const vec4 & vec) const
        {
            return x > vec.x && y > vec.y && z > vec.z && w > vec.w;
        }

        bool operator <=(const vec4 & vec) const
        {
            return x <= vec.x && y <= vec.y && z <= vec.z && w <= vec.w;
        }

        bool operator >=(const vec4 & vec) const
        {
            return x >= vec.x && y >= vec.y && z >= vec.z && w >= vec.w;
        }

    public:
        union
        {
            struct
            {
                vec2<ValT> xy;
                vec2<ValT> zw;
            };

            struct
            {
                ValT x;
                ValT y;
                ValT z;
                ValT w;
            };
        };
    };
}}
