#pragma once

#include "xm/xm.h"
#include "float4.h"
#include "quat4.h"

namespace core { namespace math { namespace xmm
{
    struct float4x4
    {
        float4x4()
        {
        }

        float4x4(const float4x4 & another) : row0(another.row0), row1(another.row1), row2(another.row2),
                                             row3(another.row3)
        {
        }

        float4x4(float4 _row0, float4 _row1, float4 _row2, float4 _row3) : row0(_row0), row1(_row1), row2(_row2),
                                                                           row3(_row3)
        {
        }

        float4x4(float4 rows[4]) : row0(rows[0]), row1(rows[1]), row2(rows[2]), row3(rows[3])
        {
        }

        float4x4(float32_t arr[4][4]) : row0(arr[0]), row1(arr[1]), row2(arr[2]), row3(arr[3])
        {
        }

        float4x4(float32_t _00, float32_t _01, float32_t _02, float32_t _03,
                 float32_t _10, float32_t _11, float32_t _12, float32_t _13,
                 float32_t _20, float32_t _21, float32_t _22, float32_t _23,
                 float32_t _30, float32_t _31, float32_t _32, float32_t _33) :
            row0(_00, _01, _02, _03),
            row1(_10, _11, _12, _13),
            row2(_20, _21, _22, _23),
            row3(_30, _31, _32, _33)
        {
        }

        float4x4(const xmm & matrix) :
            row0(matrix.row0), row1(matrix.row1), row2(matrix.row2), row3(matrix.row3)
        {
        }

        float4 & operator[](int32_t iIndex) { return ((float4 *)this)[iIndex]; }
        const float4 & operator[](int32_t iIndex) const { return ((const float4 *)this)[iIndex]; }

        float4x4 & operator=(const float4x4 & another)
        {
            row0 = another.row0;
            row1 = another.row1;
            row2 = another.row2;
            row3 = another.row3;
            return *this;
        }

        float4x4 & operator=(const xmm & matrix)
        {
            row0 = matrix.row0;
            row1 = matrix.row1;
            row2 = matrix.row2;
            row3 = matrix.row3;
            return *this;
        }

        operator xmm() const
        {
            xmm matrix;
            matrix.row0 = row0;
            matrix.row1 = row1;
            matrix.row2 = row2;
            matrix.row3 = row3;
            return matrix;
        }

    public:
        float4x4 & identity();
        float4x4 & invert();
        float4x4 & transpose();
        float4x4 & multiply(const float4x4 & another);
        float4x4 & transform(const float3 & pos, const float3 & rot, const float3 & scl);
        float4x4 & transform(const float3 & pos, const quat4 & rot, const float3 & scl);

        float4x4 & translate(const float3 & pos);

        float4x4 & rotate(const float3 & rot);
        float4x4 & rotate(const quat4 & rot);
        float4x4 & rotate_axis(const float4 & axis, float32_t angle);

        float4x4 & scale(const float3 & scale);

        float4x4 & perspective_lh(float32_t fovy, float32_t aspect, float32_t z_near, float32_t z_far);
        float4x4 & perspective_rh(float32_t fovy, float32_t aspect, float32_t z_near, float32_t z_far);

        void transform_coord(float3 & coord) const;
        void transform_coord(float4 & coord) const;
        void transform_quat(quat4 & quat) const;

        float4x4 operator *(const float4x4 & another) const;
        float4x4 & operator *=(const float4x4 & another);

        void invert(float4x4 & result) const;
        void transpose(float4x4 & result) const;
    public:
        union
        {
            float1 af[16];

            struct
            {
                float4 rows[4];
            };

            struct
            {
                float4 row0;
                float4 row1;
                float4 row2;
                float4 row3;
            };

            struct
            {
                float32_t _11, _12, _13, _14;
                float32_t _21, _22, _23, _24;
                float32_t _31, _32, _33, _34;
                float32_t _41, _42, _43, _44;
            };
        };
    };

    const float4x4 f4x4_identity = {xmf_row0, xmf_row1, xmf_row2, xmf_row3};

    //! 初始化为单位矩阵
    inline void float4x4_identity(float4x4 & matrix)
    {
        matrix = f4x4_identity;
    }

    //! 矩阵相乘
    inline float4x4 float4x4_multiply(const float4x4 & matr1, const float4x4 & matr2)
    {
        return xm_matr_mul(matr1, matr2);
    }

    //! 矩阵转置
    inline float4x4 float4x4_transpose(const float4x4 & matrix)
    {
        return xm_matr_transpose(matrix);
    }

    //! 矩阵求逆
    inline float4x4 float4x4_invert(const float4x4 & matrix, float4 * pDeterminant)
    {
        xmf determinant;
        xmm m = xm_matr_invert(matrix, &determinant);
        if (pDeterminant)
            *pDeterminant = determinant;
        return m;
    }

    //! 获取一个平移矩阵
    inline float4x4 float4x4_translate(const float3 & pos)
    {
        return xm_matr_translate(pos);
    }

    //! inline（xyzw）
    inline float4x4 float4x4_rotate(const float3 & rot)
    {
        return xm_matr_rotate(rot);
    }

    //! 获取一个旋转矩阵（四元数）
    inline float4x4 float4x4_rotation_quat(const quat4 & quat)
    {
        return xm_matr_rotate_quat(quat);
    }

    //! 获取一个绕固定轴的旋转矩阵
    inline float4x4 float4x4_rotation_axis(const float4 & axis, float32_t angle)
    {
        return xm_matr_rotate_axis(axis, angle);
    }

    //! 获取一个缩放矩阵（xyz）
    inline float4x4 float4x4_scale(const float3 & scl)
    {
        return xm_matr_scale(scl);
    }

    //! 透视矩阵
    inline float4x4 float4x4_perspective_lh(float32_t fovy, float32_t aspect, float32_t z_near, float32_t z_far)
    {
        return xmm_matr_perspective_lh(fovy, aspect, z_near, z_far);
    }

    inline float4x4 float4x4_perspective_rh(float32_t fovy, float32_t aspect, float32_t z_near, float32_t z_far)
    {
        return xmm_matr_perspective_rh(fovy, aspect, z_near, z_far);
    }

    inline float4x4 float4x4_orthogona_lh(float32_t width, float32_t height, float32_t z_near, float32_t z_far)
    {
        return xm_matr_orthogona_lh(width, height, z_near, z_far);
    }

    inline float4x4 float4x4_orthogona_rh(float32_t width, float32_t height, float32_t z_near, float32_t z_far)
    {
        return xm_matr_orthogona_rh(width, height, z_near, z_far);
    }


    inline float4x4 float4x4_lookat_lh(const float4 & eye, const float4 & at, const float4 & up)
    {
        return xm_matr_lookat_lh(eye, at, up);
    }

    inline float4x4 float4x4_lookat_rh(const float4 & eye, const float4 & at, const float4 & up)
    {
        return xm_matr_lookat_rh(eye, at, up);
    }

    inline float4x4 float4x4_lookto_lh(const float4 & eye, const float4 & direction, const float4 & up)
    {
        return xm_matr_lookto_lh(eye, direction, up);
    }

    inline float4x4 float4x4_lookto_rh(const float4 & eye, const float4 & direction, const float4 & up)
    {
        return xm_matr_lookto_rh(eye, direction, up);
    }

    inline float4x4 float4x4_transform(const float3 & pos, const float3 & rot, const float3 & scl)
    {
        float4 angles(rot, 0.0f);
        quat4 quat = xmf_quat_rot_from_angles(angles);
        return xm_matr_transform(pos, quat, scl);
    }

    inline float4x4 float4x4_transform(const float3 & pos, const quat4 & rot, const float3 & scl)
    {
        return xm_matr_transform(pos, rot, scl);
    }
}}}
