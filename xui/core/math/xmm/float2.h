#pragma once

#include "xm/xm.h"

namespace core { namespace math { namespace xmm
{
    struct float2
    {
        inline float2() : x(0), y(0)
        {
        }

        inline float2(const float2 & another) : x(another.x), y(another.y)
        {
        }

        inline float2(const xmf & v) : x(xmv_x(v)), y(xmv_y(v))
        {
        }

        inline float2(float32_t _x, float32_t _y) : x(_x), y(_y)
        {
        }

        inline float2(const float32_t (&arr)[2]) : x(arr[0]), y(arr[1])
        {
        }

        inline float2 & operator=(const float32_t (&arr)[2])
        {
            x = arr[0];
            y = arr[0];
            return *this;
        }

        inline float32_t & operator[](int32_t iIndex) { return ((float32_t *)this)[iIndex]; }
        inline const float32_t & operator[](int32_t iIndex) const { return ((const float32_t *)this)[iIndex]; }

        inline operator xmf() const
        {
            return xmf(x, y, 0.0f, 0.0f);
        }

        inline float2 & operator =(xmf v)
        {
            x = xmv_x(v);
            y = xmv_y(v);
            return *this;
        }

        inline float2 & operator =(const float2 & another)
        {
            x = another.x;
            y = another.y;
            return *this;
        }

        //----------------------------------
        float2 & set(float32_t _x, float32_t _y)
        {
            x = _x;
            y = _y;
            return *this;
        }

        float2 & set(const float32_t (&arr)[2])
        {
            x = arr[0];
            y = arr[1];
            return *this;
        }


        float2 operator +() const;
        float2 operator -() const;

        float2 operator +(const float2 & vec) const;
        float2 & operator +=(const float2 & vec);

        float2 operator -(const float2 & vec) const;
        float2 & operator -=(const float2 & vec);

        float2 operator *(float32_t rate) const;
        float2 operator *(const float2 & vec) const;
        float2 & operator *=(float32_t rate);
        float2 & operator *=(const float2 & vec);

        float2 operator /(float32_t rate) const;
        float2 operator /(const float2 & vec) const;
        float2 & operator /=(float32_t rate);
        float2 & operator /=(const float2 & vec);

        float2 & lerp(const float2 & min, const float2 & max, float32_t lerp);
    public:
        union
        {
            float32_t af[2];

            struct
            {
                float32_t x;
                float32_t y;
            };
        };
    };

    inline float2 operator *(float1 rate, float2 vec)
    {
        return vec * rate;
    }

    inline float2 operator /(float1 rate, float2 vec)
    {
        return vec / rate;
    }
}}}
