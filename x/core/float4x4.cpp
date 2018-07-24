#include "stdafx.h"
#include "float4x4.h"
#include "math/xmm.h"

namespace core
{
    using namespace math::xm;

    float4x4 & float4x4::identity()
    {
        *this = f4x4_identity;
        return *this;
    }

    float4x4 & float4x4::invert()
    {
        *this = xm_matr_invert(*this, nullptr);
        return *this;
    }

    float4x4 & float4x4::transpose()
    {
        *this = xm_matr_transpose(*this);
        return *this;
    }

    float4x4 & float4x4::multiply(const float4x4 & another)
    {
        *this = xm_matr_mul(*this, another);
        return *this;
    }

    float4x4 & float4x4::transform(const float3 & pos, const float3 & rot, const float3 & scl)
    {
        float4 angles(rot, 0.0f);
        quat4 quat = xmf_quat_rot_from_angles(quat);
        *this = xm_matr_transform(pos, quat, scl);
        return *this;
    }

    float4x4 & float4x4::transform(const float3 & pos, const quat4 & rot, const float3 & scl)
    {
        *this = xm_matr_transform(pos, rot, scl);
        return *this;
    }

    float4x4 & float4x4::translate(const float3 & pos)
    {
        *this = xm_matr_translate(pos);
        return *this;
    }

    float4x4 & float4x4::rotate(const float3 & rot)
    {
        *this = xm_matr_rotate(rot);
        return *this;
    }

    float4x4 & float4x4::rotate(const quat4 & rot)
    {
        *this = xm_matr_rotate_quat(rot);
        return *this;
    }

    float4x4 & float4x4::rotate_axis(const float4 & axis, float32_t angle)
    {
        *this = xm_matr_rotate_axis(axis, angle);
        return *this;
    }

    float4x4 & float4x4::scale(const float3 & scale)
    {
        *this = xm_matr_scale(scale);
        return *this;
    }

    float4x4 & float4x4::perspective_lh(float32_t fovy, float32_t aspect, float32_t z_near, float32_t z_far)
    {
        *this = xmm_matr_perspective_lh(fovy, aspect, z_near, z_far);
        return *this;
    }

    float4x4 & float4x4::perspective_rh(float32_t fovy, float32_t aspect, float32_t z_near, float32_t z_far)
    {
        *this = xmm_matr_perspective_rh(fovy, aspect, z_near, z_far);
        return *this;
    }

    void float4x4::transform_coord(float3 & coord) const
    {
        coord = xm_matr_transform_coord(*this, coord);
    }

    void float4x4::transform_quat(quat4 & quat) const
    {
        quat = xm_matr_transform_coord(*this, quat);
    }

    void float4x4::transform_coord(float4 & coord) const
    {
        coord = xm_matr_transform_coord(*this, coord);
    }

    float4x4 float4x4::operator *(const float4x4 & another) const
    {
        return xm_matr_mul(*this, another);
    }

    float4x4 & float4x4::operator *=(const float4x4 & another)
    {
        *this = xm_matr_mul(*this, another);
        return *this;
    }

    void float4x4::invert(float4x4 & result) const
    {
        result = xm_matr_invert(*this, nullptr);
    }

    void float4x4::transpose(float4x4 & result) const
    {
        result = xm_matr_transpose(*this);
    }
}
