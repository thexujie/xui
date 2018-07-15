#include "stdafx.h"
#include "quat4.h"
#include "xmm.h"

namespace core::math::xmm
{
    float32_t quat4::length() const
    {
        return xmv_x(xm_vec_length(*this));
    }

    float32_t quat4::length_sq() const
    {
        return xmv_x(xm_vec_length_sq(*this));
    }

    float32_t quat4::dot(const quat4 & vec) const
    {
        return xmv_x(xm_vec_dot(*this, vec));
    }

    quat4 quat4::cross(const quat4 & vec1, const quat4 & vec2) const
    {
        return xm_vec_cross(*this, vec1, vec2);
    }

    quat4 & quat4::set(float32_t _x, float32_t _y, float32_t _z, float32_t _w)
    {
        x = _x;
        y = _y;
        z = _z;
        w = _w;
        return *this;
    }

    quat4 & quat4::negate()
    {
        *this = xm_vec_negate(*this);
        return *this;
    }

    quat4 & quat4::normalize()
    {
        *this = xm_vec_normalize(*this);
        return *this;
    }

    quat4 & quat4::add(const quat4 & vec)
    {
        *this = xm_vec_add(*this, vec);
        return *this;
    }

    quat4 & quat4::sub(const quat4 & vec)
    {
        *this = xm_vec_sub(*this, vec);
        return *this;
    }

    quat4 & quat4::mul(const quat4 & vec)
    {
        *this = xm_vec_mul(*this, vec);
        return *this;
    }

    quat4 & quat4::div(const quat4 & vec)
    {
        *this = xm_vec_div(*this, vec);
        return *this;
    }

    quat4 & quat4::lerp(const quat4 & min, const quat4 & max, float32_t lerp)
    {
        *this = xm_quat_lerp(min, max, lerp);
        return *this;
    }

    quat4 quat4::operator +() const
    {
        return *this;
    }

    quat4 quat4::operator -() const
    {
        return xm_vec_negate(*this);
    }

    quat4 quat4::operator +(const quat4 & vec) const
    {
        return xm_vec_add(*this, vec);
    }

    quat4 & quat4::operator +=(const quat4 & vec)
    {
        *this = xm_vec_add(*this, vec);
        return *this;
    }

    quat4 quat4::operator -(const quat4 & vec) const
    {
        return xm_vec_sub(*this, vec);
    }

    quat4 & quat4::operator -=(const quat4 & vec)
    {
        *this = xm_vec_sub(*this, vec);
        return *this;
    }

    quat4 quat4::operator *(const quat4 & vec) const
    {
        return xm_vec_mul(*this, vec);
    }

    quat4 & quat4::operator *=(const quat4 & vec)
    {
        *this = xm_vec_mul(*this, vec);
        return *this;
    }

    quat4 quat4::operator /(const quat4 & vec) const
    {
        return xm_vec_div(*this, vec);
    }

    quat4 & quat4::operator /=(const quat4 & vec)
    {
        *this = xm_vec_div(*this, vec);
        return *this;
    }

    quat4 quat4::operator *(float32_t rate) const
    {
        return xm_vec_mul(*this, xm_vec_replicate(rate));
    }

    quat4 & quat4::operator *=(float32_t rate)
    {
        *this = xm_vec_mul(*this, xm_vec_replicate(rate));
        return *this;
    }

    quat4 quat4::operator /(float32_t rate) const
    {
        return xm_vec_div(*this, xm_vec_replicate(rate));
    }

    quat4 & quat4::operator /=(float32_t rate)
    {
        *this = xm_vec_div(*this, xm_vec_replicate(rate));
        return *this;
    }
}
