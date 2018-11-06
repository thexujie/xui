#pragma once

namespace core
{
    inline float32_t easingcurve_liner(float32_t input)
    {
        return input;
    }

    inline float32_t easingcurve_in_quad(float32_t input)
    {
        return input * input;
    }

    inline float32_t easingcurve_out_quad(float32_t input)
    {
        return 1.0f - easingcurve_in_quad(1.0f - input);
    }

    inline float32_t easingcurve_inout_quad(float32_t input)
    {
        return input < 0.5f ? easingcurve_in_quad(input) : easingcurve_out_quad(input);
    }

    inline float32_t easingcurve_outin_quad(float32_t input)
    {
        return input < 0.5f ? easingcurve_out_quad(input) : easingcurve_in_quad(input);
    }

    inline float32_t easingcurve_in_cubic(float32_t input)
    {
        return input * input * input;
    }

    inline float32_t easingcurve_out_cubic(float32_t input)
    {
        return 1.0f - easingcurve_in_cubic(1.0f - input);
    }

    inline float32_t easingcurve_inout_cubic(float32_t input)
    {
        return input < 0.5f ? easingcurve_in_cubic(input) : easingcurve_out_cubic(input);
    }

    inline float32_t easingcurve_outin_cubic(float32_t input)
    {
        return input < 0.5f ? easingcurve_out_cubic(input) : easingcurve_in_cubic(input);
    }
}
