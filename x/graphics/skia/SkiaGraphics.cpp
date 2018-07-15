#include "stdafx.h"
#include "SkiaGraphics.h"

namespace graphics::skia
{
    SkiaGraphics::SkiaGraphics()
    {

    }

    SkiaGraphics::~SkiaGraphics()
    {

    }

    void SkiaGraphics::PushOrign(core::pt32i point)
    {
        core::pt32i temp = point;
        if (!_origns.empty())
            temp += _origns.top();
        _origns.push(temp);
    }

    core::pt32i SkiaGraphics::GetOrign() const
    {
        if (_origns.empty())
            return {};
        else
            return _origns.top();
    }

    void SkiaGraphics::PopOrign()
    {
        _origns.pop();
    }

    void SkiaGraphics::PushClip(core::rc32i rect)
    {
        if (!_origns.empty())
        {
            core::pt32i orign = _origns.top();
            rect.x += orign.x;
            rect.y += orign.y;
        }
        _clips.push(rect);

        ////if (_clipRegion)
        ////{
        ////    ::SelectClipRgn(*_hdc, NULL);
        ////    ::DeleteObject(_clipRegion);
        ////    _clipRegion = NULL;
        ////}

        ////if (rect.is_valid())
        ////{
        ////    winrect_t winrect = rect;
        ////    _clipRegion = ::CreateRectRgnIndirect(&winrect);
        ////    ::SelectClipRgn(*_hdc, _clipRegion);
        ////}
        ////else
        ////    ::SelectClipRgn(*_hdc, NULL);
    }

    core::rc32i SkiaGraphics::GetClip() const
    {
        if (_clips.empty())
            return {};
        else
            return _clips.top();
    }

    void SkiaGraphics::PopClip()
    {
        _clips.pop();
        ////if (_clips.empty())
        ////    ::SelectClipRgn(*_hdc, NULL);
        ////else
        ////{
        ////    if (_clipRegion)
        ////    {
        ////        ::SelectClipRgn(*_hdc, NULL);
        ////        ::DeleteObject(_clipRegion);
        ////        _clipRegion = NULL;
        ////    }

        ////    winrect_t rect(_clips.top());
        ////    _clipRegion = ::CreateRectRgnIndirect(&rect);
        ////    ::SelectClipRgn(*_hdc, _clipRegion);
        ////}
    }

    void SkiaGraphics::Clear(core::color32 color) {}
    void SkiaGraphics::DrawString(std::string str, core::pt32i point, graphics::text::font font, core::color32 color) {}
    void SkiaGraphics::DrawImage(const graphics::IGraphicsImage & image, core::pt32i point) {}
    void SkiaGraphics::DrawImage(const graphics::IGraphicsImage & image, core::pt32i point, core::rc32i region) {}
    void SkiaGraphics::DrawImage(const graphics::IGraphicsImage & image, core::rc32i rect) {}
    void SkiaGraphics::DrawImage(const graphics::IGraphicsImage & image, core::rc32i rect, core::rc32i region) {}
    void SkiaGraphics::DrawString(const std::string & str, core::pt32i point, const text::font & font, core::color32 color, int32_t flags) {}
    void SkiaGraphics::DrawString(const std::string & str, core::rc32i rect, const text::font & font, core::color32 color, int32_t flags) {}
    void SkiaGraphics::DrawString(const graphics::IGraphicsString & str, core::pt32i point) {}
    void SkiaGraphics::FillPath(graphics::raster::path & path, core::color32 color) {}
    graphics::text::fontmetrics SkiaGraphics::GetFontMetrics(graphics::text::font font)
    {
        return {};
    }
    core::si32i SkiaGraphics::MeasureString(std::string str, graphics::text::font font)
    {
        return {};
    }
}
