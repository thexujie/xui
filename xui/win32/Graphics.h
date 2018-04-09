#pragma once
#include "graphics/GraphicsService.h"
#include "Bitmap.h"
#include "GDIObjectCache.h"
#include "win32/windows.h"

namespace win32
{
    class Graphics : public graphics::IGraphics
    {
    public:
        Graphics(std::shared_ptr<win32::Bitmap> bitmap);
        ~Graphics() = default;

        void PushOrign(core::math::pt32_t point);
        core::math::pt32_t GetOrign() const;
        void PopOrign();
        void PushClip(core::math::rc32_t rect);
        core::math::rc32_t GetClip() const;
        void PopClip();

        void Clear(core::color32 color);
        void DrawLine(core::math::pt32_t start, core::math::pt32_t end, core::color32 color, float32_t width);
        void DrawRect(core::math::rc32_t rect, core::color32 color, float32_t width);
        void FillRect(core::math::rc32_t rect, core::color32 color);

        void DrawString(std::string str, core::math::pt32_t point, graphics::text::font font, core::color32 color);

        void DrawImage(graphics::IGraphicsImage & image, core::math::pt32_t point);
        void DrawImage(graphics::IGraphicsImage & image, core::math::pt32_t point, core::math::rc32_t region);
        void DrawImage(graphics::IGraphicsImage & image, core::math::rc32_t rect);
        void DrawImage(graphics::IGraphicsImage & image, core::math::rc32_t rect, core::math::rc32_t region);
        void DrawString(graphics::IGraphicsString & str, core::math::pt32_t point);

    public:
        graphics::text::fontmetrics GetFontMetrics(graphics::text::font font);
        core::math::si32_t MeasureString(std::string str, graphics::text::font font);

    private:
        core::color32 AffineColor(core::color32 color);
        void SetPen(std::shared_ptr<HPEN> pen);
        void SetBrush(std::shared_ptr<HBRUSH> brush);
        void SetFont(std::shared_ptr<HFONT> font);
    private:
        std::shared_ptr<HDC> _hdc;
        std::shared_ptr<HDC> _hdcStaging;
        std::shared_ptr<core::Object> _bitmap;
        std::shared_ptr<GDIObjectCache> _objCache;

        HRGN _clipRegion = NULL;
        std::stack<core::math::pt32_t> _origns;
        std::stack<core::math::rc32_t> _clips;
    };
}
