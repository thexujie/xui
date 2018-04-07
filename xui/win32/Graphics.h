#pragma once
#include "graphics/GraphicsService.h"
#include "Bitmap.h"
#include "GDIObjectCache.h"

namespace win32
{
    class Graphics : public graphics::IGraphics
    {
    public:
        Graphics(std::shared_ptr<win32::Bitmap> bitmap);
        ~Graphics() = default;

        core::error_e Clear(core::color32 color);
        core::error_e DrawLine(core::math::pt32_t start, core::math::pt32_t end, core::color32 color, float32_t width);
        core::error_e DrawRect(core::math::rc32_t rect, core::color32 color, float32_t width);
        core::error_e FillRect(core::math::rc32_t rect, core::color32 color);
        core::error_e DrawString(std::string str, core::color32 color, graphics::font font, core::math::pt32_t point, int32_t flags);
        core::error_e DrawString(std::string str, core::color32 color, graphics::font font, core::math::rc32_t rect, int32_t flags);

        core::error_e DrawImage(graphics::IImage & image, core::math::pt32_t point, int32_t flags);

        core::color32 AffineColor(core::color32 color);

    private:
        handle_t _hdc = NULL;
        std::shared_ptr<core::Object> _bitmap;
        std::shared_ptr<GDIObjectCache> _objCache;
    };
}
