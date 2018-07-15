#pragma once


namespace core
{
    /**
    * @brief 四舍五入，小于 0.5f 的舍去，大于或等于 0.5f 的补齐 1.0f。
    * @return 返回取整后的 int32_t 类型值。
    */
    int32_t round(float32_t val);

    /**
    * @brief 四舍五入，小于 0.5f 的舍去，大于或等于 0.5f 的补齐 1.0f。
    * @return 返回取整后的 float32_t 类型值。
    */
    float32_t roundf(float32_t val);

    /**
    * @brief 向上舍入，不足 1.0f 的部分补齐 1.0f。
    * @return 返回取整后的 int32_t 类型值。
    */
    int32_t ceil(float32_t val);

    /**
    * @brief 向上舍入，不足 1.0f 的部分补齐 1.0f。
    * @return 返回取整后的 float32_t 类型值。
    */
    float32_t ceilf(float32_t val);

    /**
    * @brief 向下舍入，不足 1.0f 的部分舍去。
    * @return 返回取整后的 int32_t 类型值。
    */
    int32_t floor(float32_t val);
    /**
    * @brief 向下舍入，不足 1.0f 的部分舍去。
    * @return 返回取整后的 float32_t 类型值。
    */
    float32_t floorf(float32_t val);

    /**
    * @brief 判断两个 float32_t 值是否相等。
    * @return 相等返回 true，否则返回 false。
    */
    bool f32_equal(float32_t val0, float32_t val1);

    /**
    * @brief 判断一个 float32_t 值是否 小于或等于 另外一个 float32_t 值。。
    * @return 如果fa <= fb，返回 true，否则返回 false。
    */
    bool f32_less_or_equal(float32_t val0, float32_t val1);

    /**
    * @brief 判断一个 float32_t 值是否 大于或等于 另外一个 float32_t 值。。
    * @return 如果fa >= fb，返回 true，否则返回 false。
    */
    bool f32_greater_or_equal(float32_t val0, float32_t val1);

    float32_t abs(float32_t val);
    float32_t cos(float32_t val);
    float32_t sin(float32_t val);
    float32_t tan(float32_t val);
    float32_t atan(float32_t val);
    float32_t cot(float32_t val);
    float32_t sqrt(float32_t val);
    float32_t pow(float32_t val, int32_t exp);
    float32_t pow(float32_t val, float32_t exp);
    float32_t lerp(float32_t min, float32_t max, float32_t determinant);
}

#include "align.h"
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "rect.h"
#include "edge.h"

#include "math/xmm/float2.h"
#include "math/xmm/float3.h"
#include "math/xmm/float4.h"
#include "math/xmm/float4x4.h"
#include "math/xmm/quat4.h"
