#pragma once

#include "xm/xm.h"

namespace core { namespace math { namespace xmm
{
    struct quat4
    {
        quat4() : x(0), y(0), z(0), w(0)
        {
        }

        quat4(float32_t _x, float32_t _y, float32_t _z, float32_t _w) : x(_x), y(_y), z(_z), w(_w)
        {
        }

        quat4(const quat4 & another) : x(another.x), y(another.y), z(another.z), w(another.w)
        {
        }

        quat4(const xmf & v) : x(xmv_x(v)), y(xmv_y(v)), z(xmv_z(v)), w(xmv_w(v))
        {
        }

        quat4(float32_t arr[4]) : x(arr[0]), y(arr[1]), z(arr[2]), w(arr[3])
        {
        }

        float32_t & operator[](int32_t iIndex) { return af[iIndex]; }
        const float32_t & operator[](int32_t iIndex) const { return ((const float32_t *)this)[iIndex]; }

        quat4 & operator =(const quat4 & another)
        {
            x = another.x;
            y = another.y;
            z = another.z;
            w = another.w;
            return *this;
        }

        quat4 & operator =(const xmf & v)
        {
            const float32_t * value = (const float32_t *)&v;
            x = xmv_x(v);
            y = xmv_y(v);
            z = xmv_z(v);
            w = xmv_w(v);
            return *this;
        }

        quat4 & operator =(float32_t arr[4])
        {
            x = arr[0];
            y = arr[0];
            z = arr[0];
            w = arr[0];
            return *this;
        }

        operator const float32_t *() const { return af; }
        operator float32_t *() { return af; }
        operator xmf() const { return xm_vec_set(x, y, z, w); }

        float32_t length() const;
        float32_t length_sq() const;
        float32_t dot(const quat4 & vec) const;
        quat4 cross(const quat4 & vec1, const quat4 & vec2) const;

        quat4 & set(float32_t _x, float32_t _y, float32_t _z, float32_t _w);
        quat4 & negate();
        quat4 & normalize();

        quat4 & add(const quat4 & vec);
        quat4 & sub(const quat4 & vec);
        quat4 & mul(const quat4 & vec);
        quat4 & div(const quat4 & vec);

        quat4 & lerp(const quat4 & min, const quat4 & max, float32_t lerp);

        quat4 operator +() const;
        quat4 operator -() const;

        quat4 operator +(const quat4 & vec) const;
        quat4 & operator +=(const quat4 & vec);

        quat4 operator -(const quat4 & vec) const;
        quat4 & operator -=(const quat4 & vec);

        quat4 operator *(const quat4 & vec) const;
        quat4 & operator *=(const quat4 & vec);

        quat4 operator /(const quat4 & vec) const;
        quat4 & operator /=(const quat4 & vec);

        quat4 operator *(float32_t rate) const;
        quat4 & operator *=(float32_t rate);
        quat4 operator /(float32_t rate) const;
        quat4 & operator /=(float32_t rate);

    public:
        union
        {
            float32_t af[4];

            struct
            {
                float32_t x;
                float32_t y;
                float32_t z;
                float32_t w;
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
}}}
