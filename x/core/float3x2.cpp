#include "stdafx.h"
#include "float3x2.h"
#include "math/xmm.h"

namespace core
{
    float3x2 & float3x2::identity()
    {
        sx = sy = 1.0f;
        shx = shy = 0.0f;
        tx = ty = 0.0f;
        return *this;
    }

    float3x2 float3x2::invert(const float3x2 & m)
    {
        float32_t det = m.sx * m.sy - m.shx * m.shy;
        if (std::abs(det) < math::xm::xm_eplison)
            return {};

        det = 1.0f / det;
        return float3x2(
            m.sy * det, -m.shx * det, (m.shx * m.ty - m.sy * m.tx) * det,
            -m.shy * det, m.sx * det, (m.shy * m.tx - m.sx * m.ty) * det);
    }

    float3x2 & float3x2::multiply(const float3x2 & m)
    {
        *this *= m;
        return *this;
    }

    float3x2 float3x2::operator *(const float3x2 & m) const
    {
        float32_t t0 = sx * m.sx + shy * m.shx;
        float32_t t2 = shx * m.sx + sy * m.shx;
        float32_t t4 = tx * m.sx + ty * m.shx + m.tx;

        return float3x2(
            t0, t2, t4,
            sx * m.shy + shy * m.sy,
            shx * m.shy + sy * m.sy,
            tx * m.shy + ty * m.sy + m.ty);
    }

    float3x2 & float3x2::operator *=(const float3x2 & m)
    {
        *this = *this * m;
        return *this;
    }

    float3x2 float3x2::inverted() const
    {
        const float32_t xm_eplison = 1.0e-12f;

        float32_t det = sx * sy - shx * shy;
        if (std::abs(det) < xm_eplison * xm_eplison)
            return {};

        det = 1.0f / det;
        return float3x2(
            sy * det, -shx * det, (shx * ty - sy * tx) * det,
            -shy * det, sx * det, (shy * tx - sx * ty) * det);
    }
}