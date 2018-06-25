#pragma once


namespace core
{
    /**
    * @brief �������룬С�� 0.5f ����ȥ�����ڻ���� 0.5f �Ĳ��� 1.0f��
    * @return ����ȡ����� int32_t ����ֵ��
    */
    int32_t round(float32_t val);

    /**
    * @brief �������룬С�� 0.5f ����ȥ�����ڻ���� 0.5f �Ĳ��� 1.0f��
    * @return ����ȡ����� float32_t ����ֵ��
    */
    float32_t roundf(float32_t val);

    /**
    * @brief �������룬���� 1.0f �Ĳ��ֲ��� 1.0f��
    * @return ����ȡ����� int32_t ����ֵ��
    */
    int32_t ceil(float32_t val);

    /**
    * @brief �������룬���� 1.0f �Ĳ��ֲ��� 1.0f��
    * @return ����ȡ����� float32_t ����ֵ��
    */
    float32_t ceilf(float32_t val);

    /**
    * @brief �������룬���� 1.0f �Ĳ�����ȥ��
    * @return ����ȡ����� int32_t ����ֵ��
    */
    int32_t floor(float32_t val);
    /**
    * @brief �������룬���� 1.0f �Ĳ�����ȥ��
    * @return ����ȡ����� float32_t ����ֵ��
    */
    float32_t floorf(float32_t val);

    /**
    * @brief �ж����� float32_t ֵ�Ƿ���ȡ�
    * @return ��ȷ��� true�����򷵻� false��
    */
    bool f32_equal(float32_t val0, float32_t val1);

    /**
    * @brief �ж�һ�� float32_t ֵ�Ƿ� С�ڻ���� ����һ�� float32_t ֵ����
    * @return ���fa <= fb������ true�����򷵻� false��
    */
    bool f32_less_or_equal(float32_t val0, float32_t val1);

    /**
    * @brief �ж�һ�� float32_t ֵ�Ƿ� ���ڻ���� ����һ�� float32_t ֵ����
    * @return ���fa >= fb������ true�����򷵻� false��
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
