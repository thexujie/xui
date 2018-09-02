#pragma once

namespace core
{
    struct float3x2
    {
        constexpr float3x2() :row0(1, 0, 0), row1(0, 1, 0) {}
        constexpr float3x2(float3 _row0, float3 _row1, float3 _row2) : row0(_row0), row1(_row1) { }
        constexpr float3x2(float3 rows[3]) : row0(rows[0]), row1(rows[1]) { }
        constexpr float3x2(float32_t arr[3][3]) : row0(arr[0]), row1(arr[1]) { }
        constexpr float3x2(
            float32_t _00, float32_t _01, float32_t _02,
            float32_t _10, float32_t _11, float32_t _12) :
            row0(_00, _01, _02), row1(_10, _11, _12) { }
        constexpr float3x2(const float3x2 & another) : row0(another.row0), row1(another.row1) { }

        float3 & operator[](int32_t index) { return rows[index % 2]; }
        const float3 & operator[](int32_t index) const { return rows[index % 2]; }
        float3 & row(int32_t index) { return rows[index % 2]; }
        const float3 & row(int32_t index) const { return rows[index % 2]; }
        float2 col(int32_t index) const { index %= 3;  return float2(af[index], af[index + 3]); }
        float32_t at(int32_t index) const { return af[index % 6]; }
        float32_t at(int32_t row, int32_t col) const { return rows[row % 2][col % 3]; }
        float3x2 & operator=(const float3x2 & another)
        {
            row0 = another.row0;
            row1 = another.row1;
            return *this;
        }

    public:
        float3x2 & reset();

        static float3x2 translate(const float2 & pos) { return float3x2(1, 0, pos.x, 0, 1, pos.y); }
        static float3x2 translate(const float1 & tx, const float1 & ty) { return float3x2(1, 0, tx, 0, 1, ty); }

        static float3x2 rotate(float32_t r)
        {
            float32_t s = std::sin(r);
            float32_t c = std::cos(r);
            return float3x2(c, -s, 0, s, c, 0);
        }
        static float3x2 scale(const float2 & scale) { return float3x2(scale.x, 0, 0, 0, scale.y, 0); }
        static float3x2 scale(const float1 & sx, const float1 & sy) { return float3x2(sx, 0, 0, 0, sy, 0); }
        static float3x2 invert(const float3x2 & m);

        float3x2 & multiply(const float3x2 & another);
        float2 transform_coord(const float2 & coord) const { return float2(coord.x * sx + coord.y * shx + tx, coord.x * shy + coord.y * sy + ty); }

        float3x2 operator *(const float3x2 & another) const;
        float3x2 & operator *=(const float3x2 & another);

        float3x2 invert() const;

    public:
        union
        {
            struct
            {
                float3 row0;
                float3 row1;
            };

            struct
            {
                float32_t sx, shx, tx;
                float32_t shy, sy, ty;
            };

            struct
            {
                float3 rows[2];
            };

            struct
            {
                float32_t _11, _12, _13;
                float32_t _21, _22, _23;
            };

            float1 af[6];
        };

        inline static float3x2 identity = float3x2(1, 0, 0, 0, 1, 0);
    };

    inline float2 operator * (const float2 & val, const float3x2 & m)
    {
        return m.transform_coord(val);
    }

    inline float2 & operator *= (float2 & val, const float3x2 & m)
    {
        float32_t x = val.x;
        val.x = (float32_t)((float64_t)x * m.sx + (float64_t)val.y * m.shx + m.tx);
        val.y = (float32_t)((float64_t)x * m.shy + (float64_t)val.y * m.sy + m.ty);
        return val;
    }
}