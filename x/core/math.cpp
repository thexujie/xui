#include "stdafx.h"
#include <math.h>
#include "math.h"

namespace core::math
{
    int32_t round(float32_t val)
    {
        return (int32_t)::roundf(val);
    }

    float32_t roundf(float32_t val)
    {
        return ::roundf(val);
    }

    int32_t ceil(float32_t val)
    {
        return (int32_t)::ceilf(val);
    }

    float32_t ceilf(float32_t val)
    {
        return ::ceilf(val);
    }

    int32_t floor(float32_t val)
    {
        return (int32_t)::floorf(val);
    }

    float32_t floorf(float32_t val)
    {
        return ::floorf(val);
    }

    bool f32_equal(float32_t val0, float32_t val1)
    {
        float32_t diff = val0 - val1;
        return (diff > -std::numeric_limits<float32_t>::epsilon()) && (diff < std::numeric_limits<float32_t>::epsilon());
    }

    bool f32_less_or_equal(float32_t val0, float32_t val1)
    {
        return val0 < val1 || f32_equal(val0, val1);
    }

    bool f32_greater_or_equal(float32_t val0, float32_t val1)
    {
        return val0 > val1 || f32_equal(val0, val1);
    }


    float32_t abs(float32_t val)
    {
        return ::fabsf(val);
    }

    float32_t cos(float32_t val)
    {
        return ::cosf(val);
    }

    float32_t sin(float32_t val)
    {
        return ::sinf(val);
    }

    float32_t tan(float32_t val)
    {
        return ::tanf(val);
    }

    float32_t atan(float32_t val)
    {
        return ::atanf(val);
    }

    float32_t cot(float32_t val)
    {
        return 1.0f / ::tanf(val);
    }

    float32_t sqrt(float32_t val)
    {
        return ::sqrtf(val);
    }

    float32_t pow(float32_t val, int32_t exp)
    {
        return ::powf(val, (float32_t)exp);
    }

    float32_t pow(float32_t val, float32_t exp)
    {
        return ::powf(val, exp);
    }

    float32_t lerp(float32_t min, float32_t max, float32_t determinant)
    {
        return min + (max - min) * determinant;
    }
}
