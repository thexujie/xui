#pragma once

namespace core::math
{
    template<typename ValT>
    class vec3
    {
    public:
        vec3() : x(0), y(0), z(0) { }

        vec3(ValT _x, ValT _y, ValT _z) : x(_x), y(_y), z(_z) { }

        vec3(const vec3 & vec) : x(vec.x), y(vec.y), z(vec.z) { }

        void set(ValT _x, ValT _y, ValT _z)
        {
            x = _x;
            y = _y;
            z = _z;
        }

        vec3 & operator =(const vec3 & vec)
        {
            x = vec.x;
            y = vec.y;
            z = vec.z;
            return *this;
        }

        //------------------------------------------------------- 四则运算
        vec3 operator +(const vec3 & vec) const
        {
            return vec3(x + vec.x, y + vec.y, z + vec.z);
        }

        vec3 operator -(const vec3 & vec) const
        {
            return vec3(x - vec.x, y - vec.y, z - vec.z);
        }

        vec3 operator *(const vec3 & vec) const
        {
            return vec3(x * vec.x, y * vec.y, z * vec.z);
        }

        vec3 operator /(const vec3 & vec) const
        {
            return vec3(x / vec.x, y / vec.y, z / vec.z);
        }

        vec3 & operator +=(const vec3 & vec)
        {
            x += vec.x;
            y += vec.y;
            z += vec.z;
            return *this;
        }

        vec3 & operator -=(const vec3 & vec)
        {
            x -= vec.x;
            y -= vec.y;
            z -= vec.z;
            return *this;
        }

        vec3 & operator *=(const vec3 & vec)
        {
            x *= vec.x;
            y *= vec.y;
            z *= vec.z;
            return *this;
        }

        vec3 & operator /=(const vec3 & vec)
        {
            x /= vec.x;
            y /= vec.y;
            z /= vec.z;
            return *this;
        }

        vec3 operator *(const ValT & rate) const
        {
            return vec3(x * rate, y * rate, z * rate);
        }

        vec3 operator /(const ValT & rate)
        {
            return vec3(x / rate, y / rate, z / rate);
        }

        vec3 & operator *=(const ValT & rate)
        {
            x *= rate;
            y *= rate;
            z *= rate;
            return *this;
        }

        vec3 & operator /=(const ValT & rate)
        {
            x /= rate;
            y /= rate;
            z /= rate;
            return *this;
        }

        vec3 operator -() const
        {
            return vec3(-x, -y, -z);
        }

        vec3 operator +() const
        {
            return *this;
        }

        //--------------------------------------------------比较运算
        bool operator ==(const vec3 & vec) const
        {
            return x == vec.x && y == vec.y && z == vec.z;
        }

        bool operator !=(const vec3 & vec) const
        {
            return x != vec.x || y != vec.y || z != vec.z;
        }

        bool operator <(const vec3 & vec) const
        {
            return x < vec.x && y < vec.y && z < vec.z;
        }

        bool operator >(const vec3 & vec) const
        {
            return x > vec.x && y > vec.y && z > vec.z;
        }

        bool operator <=(const vec3 & vec) const
        {
            return x <= vec.x && y <= vec.y && z <= vec.z;
        }

        bool operator >=(const vec3 & vec) const
        {
            return x >= vec.x && y >= vec.y && z >= vec.z;
        }

    public:
        union
        {
            struct
            {
                vec2<ValT> xy;
                ValT __z;
            };

            struct
            {
                ValT __x;
                vec2<ValT> yz;
            };

            struct
            {
                ValT x;
                ValT y;
                ValT z;
            };
        };
    };

    typedef vec3<int32_t> intx4;
    //typedef vec3x1<float32_t> float2x1;
}
