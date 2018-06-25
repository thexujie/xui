#pragma once
#include "graphics/GraphicsService.h"
#include "graphics/raster/agg.h"
#include "Bitmap.h"
#include "GDIObjectCache.h"
#include "win32/windows.h"
#include "uniscribe/script.h"

namespace win32
{
    class Graphics : public graphics::IGraphics
    {
    public:
        Graphics(std::shared_ptr<win32::Bitmap> bitmap);
        ~Graphics() = default;

        void PushOrign(core::pt32i point);
        core::pt32i GetOrign() const;
        void PopOrign();
        void PushClip(core::rc32i rect);
        core::rc32i GetClip() const;
        void PopClip();

        void Clear(core::color32 color);

        void DrawString(std::string str, core::pt32i point, graphics::text::font font, core::color32 color);

        void DrawImage(const graphics::IGraphicsImage & image, core::pt32i point);
        void DrawImage(const graphics::IGraphicsImage & image, core::pt32i point, core::rc32i region);
        void DrawImage(const graphics::IGraphicsImage & image, core::rc32i rect);
        void DrawImage(const graphics::IGraphicsImage & image, core::rc32i rect, core::rc32i region);

        void DrawString(const std::string & str, core::pt32i point, const graphics::text::font & font, core::color32 color, int32_t flags);
        void DrawString(const std::string & str, core::rc32i rect, const graphics::text::font & font, core::color32 color, int32_t flags);
        void DrawString(graphics::IGraphicsString & str, core::pt32i point);

    public:
        void FillPath(graphics::raster::path & path, core::color32 color);
        graphics::text::fontmetrics GetFontMetrics(graphics::text::font font);
        core::si32i MeasureString(std::string str, graphics::text::font font);

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
        std::stack<core::pt32i> _origns;
        std::stack<core::rc32i> _clips;

        agg::rasterizer_scanline_aa<> _raster;
        agg::scanline_u8 _sl;

        agg::rendering_buffer _rbuf;

        win32::uniscribe::ScriptItem _scpItemTemp;
    };
}
