#pragma once

#include "float3.h"

namespace core
{
    struct float3x3
    {
        constexpr float3x3() :row0(), row1(), row2() {}
        constexpr float3x3(float3 _row0, float3 _row1, float3 _row2) : row0(_row0), row1(_row1), row2(_row2) { }
        constexpr float3x3(float3 rows[3]) : row0(rows[0]), row1(rows[1]), row2(rows[2]) { }
        constexpr float3x3(float32_t arr[3][3]) : row0(arr[0]), row1(arr[1]), row2(arr[2])  { }
        constexpr float3x3(
            float32_t _00, float32_t _01, float32_t _02,
            float32_t _10, float32_t _11, float32_t _12,
            float32_t _20, float32_t _21, float32_t _22) :
            row0(_00, _01, _02), row1(_10, _11, _12), row2(_20, _21, _22) { }
        constexpr float3x3(const math::xm::xmm & matrix) : row0(matrix.row0), row1(matrix.row1), row2(matrix.row2) { }
        constexpr float3x3(const float3x3 & another) : row0(another.row0), row1(another.row1), row2(another.row2) { }

        float3 & operator[](int32_t index) { return rows[index]; }
        const float3 & operator[](int32_t index) const { return rows[index]; }

        float3x3 & operator=(const float3x3 & another)
        {
            row0 = another.row0;
            row1 = another.row1;
            row2 = another.row2;
            return *this;
        }

        float3x3 & operator=(const math::xm::xmm & matrix)
        {
            row0 = matrix.row0;
            row1 = matrix.row1;
            row2 = matrix.row2;
            return *this;
        }

        operator math::xm::xmm() const
        {
            math::xm::xmm matrix;
            matrix.row0 = row0;
            matrix.row1 = row1;
            matrix.row2 = row2;
            return matrix;
        }

    public:
        float3x3 & reset();
        float3x3 & invert();
        float3x3 & transpose();
        float3x3 & multiply(const float3x3 & another);
        float3x3 & transform(const float3 & pos, const float3 & rot, const float3 & scl);
        float3x3 & transform(const float3 & pos, const quat4 & rot, const float3 & scl);

        float3x3 & translate(const float3 & pos);

        float3x3 & rotate(const float3 & rot);
        float3x3 & rotate(const quat4 & rot);
        float3x3 & rotate_axis(const float4 & axis, float32_t angle);

        float3x3 & scale(const float3 & scale);

        float3x3 & perspective_lh(float32_t fovy, float32_t aspect, float32_t z_near, float32_t z_far);
        float3x3 & perspective_rh(float32_t fovy, float32_t aspect, float32_t z_near, float32_t z_far);

        void transform_coord(float3 & coord) const;
        void transform_quat(quat4 & quat) const;

        float3x3 operator *(const float3x3 & another) const;
        float3x3 & operator *=(const float3x3 & another);

        void invert(float3x3 & result) const;
        void transpose(float3x3 & result) const;
    public:
        union
        {
            float1 af[12];

            struct
            {
                float3 rows[3];
            };

            struct
            {
                float3 row0;
                float3 row1;
                float3 row2;
            };

            struct
            {
                float32_t _11, _12, _13;
                float32_t _21, _22, _23;
                float32_t _31, _32, _33;
            };
        };

        inline static float3x3 identity = float3x3(1, 0, 0, 0, 1, 0, 1, 0, 0);
    };
}
