#pragma once

#include "float2.h"

namespace core
{
    struct float3
    {
        inline float3() : x(0), y(0), z(0) { }

        inline float3(const float3 & another) : x(another.x), y(another.y), z(another.z) { }

        inline float3(const math::xm::xmf & m128) : x(xmv_x(m128)), y(xmv_y(m128)), z(xmv_z(m128)) { }

        inline float3(float32_t _x, float32_t _y, float32_t _z) : x(_x), y(_y), z(_z) { }

        inline float3(const float32_t (&arr)[3]) : x(arr[0]), y(arr[1]), z(arr[2]) { }

        inline float3 & operator=(const float32_t (&arr)[3])
        {
            x = arr[0];
            y = arr[0];
            z = arr[0];
            return *this;
        }

        inline float32_t & operator[](int32_t iIndex) { return ((float32_t *)this)[iIndex]; }
        inline const float32_t & operator[](int32_t iIndex) const { return ((const float32_t *)this)[iIndex]; }

        inline operator math::xm::xmf() const { return math::xm::xmf(x, y, z, 0.0f); }

        inline float3 & operator =(const math::xm::xmf & v)
        {
            x = math::xm::xmv_x(v);
            y = math::xm::xmv_y(v);
            z = math::xm::xmv_z(v);
            return *this;
        }

        inline float3 & operator =(const float3 & another)
        {
            x = another.x;
            y = another.y;
            z = another.z;
            return *this;
        }

        // -----------------------------------------------------------
        inline float3 & set(float32_t _x, float32_t _y, float32_t _z)
        {
            x = _x;
            y = _y;
            z = _z;
            return *this;
        }

        inline float3 & set(const float32_t (&arr)[3])
        {
            x = arr[0];
            y = arr[0];
            z = arr[0];
            return *this;
        }

        float32_t length() const;
        float32_t length_sq() const;
        float32_t dot(const float3 & vec) const;
        float3 cross(const float3 & vec) const;
        float3 abs() const;

        float3 & negate();
        float3 & normalize();
        float3 & add(const float3 & vec);
        float3 & sub(const float3 & vec);
        float3 & mul(const float3 & vec);
        float3 & div(const float3 & vec);

        float3 operator +() const;
        float3 operator -() const;

        float3 operator +(const float3 & vec) const;
        float3 & operator +=(const float3 & vec);

        float3 operator -(const float3 & vec) const;
        float3 & operator -=(const float3 & vec);

        float3 operator *(float32_t rate) const;
        float3 operator *(const float3 & vec) const;
        float3 & operator *=(float32_t rate);
        float3 & operator *=(const float3 & vec);

        float3 operator /(float32_t rate) const;
        float3 operator /(const float3 & vec) const;
        float3 & operator /=(float32_t rate);
        float3 & operator /=(const float3 & vec);

        inline bool operator ==(const float3 & vec) const
        {
            math::xm::xmf diff = math::xm::xm_vec_abs(math::xm::xm_vec_sub(*this, vec));
            return !!math::xm::xmi_x(xm_vec_less(diff, math::xm::xmf_epsilon));
        }

        inline bool operator !=(const float3 & vec) const
        {
            return !operator==(vec);
        }

        float3 & lerp(const float3 & min, const float3 & max, float32_t lerp);

    public:
        union
        {
            float32_t af[3];

            struct
            {
                float32_t x;
                float32_t y;
                float32_t z;
            };

            struct
            {
                float2 xy;
                float1 __z;
            };

            struct
            {
                float2 __x;
                float1 yz;
            };
        };
    };

    inline float3 operator *(float1 rate, float3 vec)
    {
        return vec * rate;
    }

    inline float3 operator /(float1 rate, float3 vec)
    {
        return vec / rate;
    }
}
