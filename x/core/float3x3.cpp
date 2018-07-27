#include "stdafx.h"
#include "float3x3.h"
#include "math/xmm.h"

namespace core
{
    using namespace math::xm;

    float3x3 & float3x3::reset()
    {
        *this = identity;
        return *this;
    }

    float3x3 & float3x3::invert()
    {
        *this = xm_matr_invert(*this, nullptr);
        return *this;
    }

    float3x3 & float3x3::transpose()
    {
        *this = xm_matr_transpose(*this);
        return *this;
    }

    float3x3 & float3x3::multiply(const float3x3 & another)
    {
        *this = xm_matr_mul(*this, another);
        return *this;
    }

    float3x3 & float3x3::transform(const float3 & pos, const float3 & rot, const float3 & scl)
    {
        float4 angles(rot, 0.0f);
        quat4 quat = xmf_quat_rot_from_angles(quat);
        *this = xm_matr_transform(pos, quat, scl);
        return *this;
    }

    float3x3 & float3x3::transform(const float3 & pos, const quat4 & rot, const float3 & scl)
    {
        *this = xm_matr_transform(pos, rot, scl);
        return *this;
    }

    float3x3 & float3x3::translate(const float3 & pos)
    {
        *this = xm_matr_translate(pos);
        return *this;
    }

    float3x3 & float3x3::rotate(const float3 & rot)
    {
        *this = xm_matr_rotate(rot);
        return *this;
    }

    float3x3 & float3x3::rotate(const quat4 & rot)
    {
        *this = xm_matr_rotate_quat(rot);
        return *this;
    }

    float3x3 & float3x3::rotate_axis(const float4 & axis, float32_t angle)
    {
        *this = xm_matr_rotate_axis(axis, angle);
        return *this;
    }

    float3x3 & float3x3::scale(const float3 & scale)
    {
        *this = xm_matr_scale(scale);
        return *this;
    }

    float3x3 & float3x3::perspective_lh(float32_t fovy, float32_t aspect, float32_t z_near, float32_t z_far)
    {
        *this = xmm_matr_perspective_lh(fovy, aspect, z_near, z_far);
        return *this;
    }

    float3x3 & float3x3::perspective_rh(float32_t fovy, float32_t aspect, float32_t z_near, float32_t z_far)
    {
        *this = xmm_matr_perspective_rh(fovy, aspect, z_near, z_far);
        return *this;
    }

    void float3x3::transform_coord(float3 & coord) const
    {
        coord = xm_matr_transform_coord(*this, coord);
    }

    void float3x3::transform_quat(quat4 & quat) const
    {
        quat = xm_matr_transform_coord(*this, quat);
    }

    float3x3 float3x3::operator *(const float3x3 & another) const
    {
        return xm_matr_mul(*this, another);
    }

    float3x3 & float3x3::operator *=(const float3x3 & another)
    {
        *this = xm_matr_mul(*this, another);
        return *this;
    }

    void float3x3::invert(float3x3 & result) const
    {
        result = xm_matr_invert(*this, nullptr);
    }

    void float3x3::transpose(float3x3 & result) const
    {
        result = xm_matr_transpose(*this);
    }
}
