#pragma once

#include "float3.h"

namespace core
{
    struct float4x4;

    struct float4
    {
        constexpr float4() : x(0), y(0), z(0), w(0) { }
        constexpr float4(float32_t _x, float32_t _y, float32_t _z, float32_t _w) : x(_x), y(_y), z(_z), w(_w) { }
        constexpr float4(const float3 & another) : x(another.x), y(another.y), z(another.z), w(0) { }
        constexpr float4(const float3 & another, float32_t _w) : x(another.x), y(another.y), z(another.z), w(_w) { }
        constexpr float4(const math::xm::xmf & v) : x(xmv_x(v)), y(xmv_y(v)), z(xmv_z(v)), w(xmv_w(v)) { }
        constexpr float4(const float32_t (&arr)[4]) : x(arr[0]), y(arr[1]), z(arr[2]), w(arr[3]) { }
        constexpr float4(const float4 & another) : x(another.x), y(another.y), z(another.z), w(another.w) { }

        float32_t & operator[](int32_t iIndex) { return af[iIndex]; }
        const float32_t & operator[](int32_t iIndex) const { return ((const float32_t *)this)[iIndex]; }

        float4 & operator =(const float4 & another)
        {
            x = another.x;
            y = another.y;
            z = another.z;
            w = another.w;
            return *this;
        }

        float4 & operator =(const math::xm::xmf & v)
        {
            const float32_t * value = (const float32_t *)&v;
            x = xmv_x(v);
            y = xmv_y(v);
            z = xmv_z(v);
            w = xmv_w(v);
            return *this;
        }

        float4 & operator =(const float32_t (&arr)[4])
        {
            x = arr[0];
            y = arr[1];
            z = arr[2];
            w = arr[3];
            return *this;
        }

        operator const float32_t *() const { return af; }
        operator float32_t *() { return af; }
        operator math::xm::xmf() const { return math::xm::xm_vec_set(x, y, z, w); }
        operator float3() const { return xyz; }

        float32_t length() const;
        float32_t length_sq() const;
        float32_t dot(const float4 & vec) const;
        float4 cross(const float4 & vec1, const float4 & vec2) const;

        float4 & set(float32_t _x, float32_t _y, float32_t _z, float32_t _w)
        {
            x = _x;
            y = _y;
            z = _z;
            w = _w;
            return *this;
        }

        float4 & set(const float32_t (&arr)[4])
        {
            x = arr[0];
            y = arr[0];
            z = arr[0];
            w = arr[0];
            return *this;
        }

        float4 & negate();
        float4 & normalize();

        float4 & add(const float4 & vec);
        float4 & sub(const float4 & vec);
        float4 & mul(const float4 & vec);
        float4 & div(const float4 & vec);

        float4 & lerp(const float4 & min, const float4 & max, float32_t lerp);

        float4 operator +() const;
        float4 operator -() const;

        float4 operator +(const float4 & vec) const;
        float4 & operator +=(const float4 & vec);

        float4 operator -(const float4 & vec) const;
        float4 & operator -=(const float4 & vec);

        float4 operator *(const float4 & vec) const;
        float4 & operator *=(const float4 & vec);

        float4 operator /(const float4 & vec) const;
        float4 & operator /=(const float4 & vec);

        float4 operator *(float32_t rate) const;
        float4 & operator *=(float32_t rate);
        float4 operator /(float32_t rate) const;
        float4 & operator /=(float32_t rate);

    public:
        union
        {
            float32_t af[4];

            union
            {
                struct
                {
                    float3 xyz;
                    float32_t __w;
                };

                struct
                {
                    float32_t x;
                    float32_t y;
                    float32_t z;
                    float32_t w;
                };

                struct
                {
                    float32_t left;
                    float32_t top;
                    float32_t right;
                    float32_t bottom;
                };
                struct
                {
                    float2 leftTop;
                    float2 rightBottom;
                };
            };

            int32_t ai[4];

            struct
            {
                int32_t ix;
                int32_t iy;
                int32_t iz;
                int32_t iw;
            };

            uint32_t aui[4];

            struct
            {
                uint32_t uix;
                uint32_t uiy;
                uint32_t uiz;
                uint32_t uiw;
            };
        };
    };

    inline float4 operator *(float1 rate, float4 vec)
    {
        return vec * rate;
    }

    inline float4 operator /(float1 rate, float4 vec)
    {
        return vec / rate;
    }

    constexpr static float4 _1000 = { 1, 0, 0, 0 };
    constexpr static float4 _0100 = { 0, 1, 0, 0 };
    constexpr static float4 _0010 = { 0, 0, 1, 0 };
    constexpr static float4 _0001 = { 0, 0, 0, 1 };
}
