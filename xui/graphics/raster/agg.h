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
#include "agg/agg_bounding_rect.h"
#include "agg/agg_trans_perspective.h"
#include "agg/agg_blur.h"
#include "agg/agg_path_storage.h"

#include "agg/agg_span_allocator.h"
#include "agg/agg_span_interpolator_linear.h"
#include "agg/agg_span_interpolator_persp.h"
#include "agg/agg_span_interpolator_trans.h"

#include "agg/agg_ellipse.h"
#include "agg/agg_rounded_rect.h"
#include "path_adapter.h"

#include "span_image_filter_rgb_bilinear_clip.h"

namespace agg::tools
{
    //inline agg::rgba rgba(core::color32 color)
    //{
    //    return agg::rgba{ color.r / 255.0, color.g / 255.0, color.b / 255.0, color.a / 255.0 };
    //}
}
