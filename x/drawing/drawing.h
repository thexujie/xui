#pragma once

#include "core/core.h"

namespace drawing
{
    struct bitmap_buffer
    {
        byte_t * data = nullptr;
        core::si32i size;
        int32_t strike = 0;
        int32_t pitch = 0;
        bool flip_y = false;
    };
}

#include "drawing/image/image.h"
#include "drawing/font.h"

#include "drawing/PathStyle.h"
#include "drawing/StringFormat.h"
#include "drawing/TextBlob.h"
#include "drawing/Bitmap.h"
#include "drawing/Image.h"
#include "drawing/Path.h"
#include "drawing/Region.h"
#include "drawing/Graphics.h"
