#pragma once
#include "graphics/GraphicsService.h"
#include "graphics/raster/agg.h"
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

        void DrawString(std::string str, core::math::pt32_t point, graphics::text::font font, core::color32 color);

        void DrawImage(const graphics::IGraphicsImage & image, core::math::pt32_t point);
        void DrawImage(const graphics::IGraphicsImage & image, core::math::pt32_t point, core::math::rc32_t region);
        void DrawImage(const graphics::IGraphicsImage & image, core::math::rc32_t rect);
        void DrawImage(const graphics::IGraphicsImage & image, core::math::rc32_t rect, core::math::rc32_t region);
        void DrawString(graphics::IGraphicsString & str, core::math::pt32_t point);

    public:
        void FillPath(graphics::raster::path & path, core::color32 color);
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

        agg::rasterizer_scanline_aa<> _raster;
        agg::scanline_u8 _sl;

        agg::render_buffer<uint8_t> _rbuf;
    };
}
