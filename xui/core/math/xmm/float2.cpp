#include "stdafx.h"
#include "float2.h"

namespace core { namespace math { namespace xmm
{
    float2 float2::operator +() const
    {
        return *this;
    }

    float2 float2::operator -() const
    {
        return xm_vec_negate(*this);
    }

    float2 float2::operator +(const float2 & vec) const
    {
        return xm_vec_add(*this, vec);
    }

    float2 & float2::operator +=(const float2 & vec)
    {
        *this = xm_vec_add(*this, vec);
        return *this;
    }

    float2 float2::operator -(const float2 & vec) const
    {
        return xm_vec_sub(*this, vec);
    }

    float2 & float2::operator -=(const float2 & vec)
    {
        *this = xm_vec_sub(*this, vec);
        return *this;
    }

    float2 float2::operator *(float32_t rate) const
    {
        return xm_vec_mul(*this, float2(rate, rate));
    }

    float2 float2::operator *(const float2 & vec) const
    {
        return xm_vec_mul(*this, vec);
    }

    float2 & float2::operator *=(float32_t rate)
    {
        *this = xm_vec_mul(*this, float2(rate, rate));
        return *this;
    }

    float2 & float2::operator *=(const float2 & vec)
    {
        *this = xm_vec_mul(*this, vec);
        return *this;
    }

    float2 & float2::lerp(const float2 & min, const float2 & max, float32_t lerp)
    {
        *this = xm_vec_lerp(min, max, lerp);
        return *this;
    }

    float2 float2::operator /(float32_t rate) const
    {
        return xm_vec_div(*this, xm_vec_replicate(rate));
    }

    float2 float2::operator /(const float2 & vec) const
    {
        return xm_vec_div(*this, vec);
    }

    float2 & float2::operator /=(float32_t rate)
    {
        *this = xm_vec_div(*this, xm_vec_replicate(rate));
        return *this;
    }

    float2 & float2::operator /=(const float2 & vec)
    {
        *this = xm_vec_div(*this, vec);
        return *this;
    }
}}}
