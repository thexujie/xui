#pragma once

#include "agg/agg_basics.h"
#include "agg/agg_rendering_buffer.h"
#include "agg/agg_rasterizer_scanline_aa.h"
#include "agg/agg_conv_transform.h"
#include "agg/agg_conv_curve.h"
#include "agg/agg_conv_contour.h"
#include "agg/agg_conv_stroke.h"
#include "agg/agg_scanline_p.h"
#include "agg/agg_scanline_u.h"
#include "agg/agg_renderer_scanline.h"
#include "agg/agg_pixfmt_rgb.h"
#include "agg/agg_pixfmt_rgba.h"
#include "agg/agg_pixfmt_gray.h"
#include "agg/agg_bounding_rect.h"
#include "agg/agg_trans_perspective.h"
#include "agg/agg_blur.h"
#include "agg/agg_path_storage.h"

#include "agg/agg_ellipse.h"
#include "agg/agg_rounded_rect.h"

namespace agg { namespace tools
{
    inline agg::rgba rgba(core::color32 color)
    {
        return agg::rgba{ color.r / 255.0, color.g / 255.0, color.b / 255.0, color.a / 255.0 };
    }
}}
