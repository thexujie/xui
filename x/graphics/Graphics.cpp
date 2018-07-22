#include "stdafx.h"
#include "Graphics.h"
#include "skia/skia.h"
#include "skia/SkShaper.h"
#include <SkScalar.h>
#include <SkTextBlob.h>

namespace graphics
{
    inline SkPaint::Style to(DrawMode style)
    {
        if (style == DrawMode::Stroke)
            return SkPaint::kStroke_Style;
        if (style == DrawMode::Fill)
            return SkPaint::kFill_Style;
        //if (style == DrawMode::StrokeFill)
            return SkPaint::kStrokeAndFill_Style;
    }

    //Graphics::Graphics(core::si32i size): _native(GraphicsService().CreateGraphics(GraphicsService().CreatePixmap(size))) { }

    Graphics::Graphics(std::shared_ptr<Bitmap> pixmap)
    {
        _pixmap = pixmap;
        _native = std::make_shared<SkCanvas>(pixmap->native());
    }

    void Graphics::Clear(core::color32 color)
    {
        if (!_native)
            return;

        _native->clear(color);
    }

    void Graphics::drawLine(core::pt32f start, core::pt32f end, const Style & style)
    {
        if (!_native)
            return;

        SkPaint paint;
        paint.setColor(style._stokeColor);
        paint.setStrokeWidth(style._width);
        paint.setStyle(to(style._mode));
        paint.setAntiAlias(style._aa);
        _native->drawLine({ start.x, start.y }, { end.x, end.y }, paint);
    }

    void Graphics::drawEllipse(core::rc32f ellipse, const Style & style)
    {
        if (!_native)
            return;

        SkPaint paint;
        paint.setColor(style._stokeColor);
        paint.setStrokeWidth(style._width);
        paint.setStyle(to(style._mode));
        paint.setAntiAlias(style._aa);
        _native->drawOval({ ellipse.x, ellipse.y, ellipse.right(), ellipse.bottom() }, paint);
    }

    void Graphics::drawRectangle(core::rc32f rect, const Style & style)
    {
        if (!_native)
            return;

        SkPaint paint;
        paint.setColor(style._stokeColor);
        paint.setStrokeWidth(style._width);
        paint.setStyle(to(style._mode));
        paint.setAntiAlias(style._aa);
        _native->drawRect({ rect.x, rect.y, rect.right(), rect.bottom()}, paint);
    }

    void Graphics::drawRoundRect(core::rc32f rect, float32_t rx, float32_t ry, const Style & style)
    {
        if (!_native)
            return;

        SkPaint paint;
        paint.setColor(style._stokeColor);
        paint.setStrokeWidth(style._width);
        paint.setStyle(to(style._mode));
        paint.setAntiAlias(style._aa);
        _native->drawRoundRect({ rect.x, rect.y, rect.right(), rect.bottom()}, rx, ry, paint);
    }

    void Graphics::drawString(const std::string & str, core::pt32f point, const StringFormat & format)
    {
        if (!_native)
            return;

        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setLCDRenderText(format._lcd);
        paint.setColor(format._color);
        paint.setTextSize(format._font.size);
        SkShaper shaper(SkTypeface::MakeFromName(format._font.family.c_str(), SkFontStyle(format._font.weight, format._font.width, (SkFontStyle::Slant)format._font.slant)));
        if (!_blobBuilder)
            _blobBuilder = std::make_shared<SkTextBlobBuilder>();
        SkRect rcBlob;
        SkPoint end = shaper.shape(*_blobBuilder, paint, str.c_str(), str.length(), true, {}, -1, rcBlob);

        if (format._align & core::align::right)
            point.x -= rcBlob.width();
        else if (format._align & core::align::centerX)
            point.x -= rcBlob.width() / 2;
        else {}

        if (format._align & core::align::bottom)
            point.y -= rcBlob.height();
        else if (format._align & core::align::centerY)
            point.y -= rcBlob.height() / 2;
        else {}

        sk_sp<SkTextBlob> blob = _blobBuilder->make();
        _native->drawTextBlob(blob, point.x, point.y, paint);
    }

    void Graphics::drawString(const std::string & str, core::rc32f rect, const StringFormat & format)
    {
        if (!_native)
            return;

        if (!_native)
            return;

        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setLCDRenderText(format._lcd);
        paint.setColor(format._color);
        paint.setTextSize(format._font.size);
        SkShaper shaper(SkTypeface::MakeFromName(format._font.family.c_str(), SkFontStyle(format._font.weight, format._font.width, (SkFontStyle::Slant)format._font.slant)));

        if (!_blobBuilder)
            _blobBuilder = std::make_shared<SkTextBlobBuilder>();
        SkRect rcBlob;
        SkPoint end = shaper.shape(*_blobBuilder, paint, str.c_str(), str.length(), true, {}, rect.cx, rcBlob);

        core::pt32f point = rect.leftTop();
        if (format._align & core::align::right)
            point.x = rect.right() - rcBlob.width();
        else if (format._align & core::align::centerX)
            point.x = (rect.cx - rcBlob.width()) / 2;
        else {}

        if (format._align & core::align::bottom)
            point.y = rect.bottom() - rcBlob.height();
        else if (format._align & core::align::centerY)
            point.y = (rect.cy - rcBlob.height()) / 2;
        else {}

        sk_sp<SkTextBlob> blob = _blobBuilder->make();
        _native->clipRect(skia::from(rect));
        _native->drawTextBlob(blob, point.x, point.y, paint);
        _native->clipRect(SkRect());
    }

    void Graphics::drawString(const IGraphicsString & str, core::pt32i point)
    {
        if (!_native)
            return;
    }

    void Graphics::drawImage(const Image & image, core::pt32f point, int32_t flags)
    {
        if (!_native)
            return;

        auto size = image.size();
        if (flags & core::align::right)
            point.x -= size.cx;
        else if (flags & core::align::centerX)
            point.x -= size.cx / 2;
        else {}

        if (flags & core::align::bottom)
            point.y -= size.cy;
        else if (flags & core::align::centerY)
            point.y -= size.cy / 2;
        else {}

        _native->drawImage(&image.native(), point.x, point.y, nullptr);
    }

    void Graphics::drawImage(const Image & image, core::rc32f rect, int32_t flags)
    {
        if (!_native)
            return;

        core::pt32i point;
        auto size = image.size();
        if (flags & core::align::right)
            point.x = rect.right() - size.cx;
        else if (flags & core::align::centerX)
            point.x = rect.centerX() - size.cx / 2;
        else
            point.x = rect.x;

        if (flags & core::align::bottom)
            point.y = rect.bottom() - size.cy;
        else if (flags & core::align::centerY)
            point.y = rect.centerY() - size.cy / 2;
        else
            point.y = rect.y;

        _native->drawImage(&image.native(), point.x, point.y, nullptr);
    }

    void Graphics::drawImage(const Image & image, core::pt32f point, core::rc32i region, int32_t flags)
    {
        if (!_native)
            return;

        if (flags & core::align::right)
            point.x = point.x - region.cx;
        else if (flags & core::align::centerX)
            point.x = point.x - region.cx / 2;
        else {}

        if (flags & core::align::bottom)
            point.y = point.y - region.cy;
        else if (flags & core::align::centerY)
            point.y = point.y - region.cy / 2;
        else {}

        _native->drawImageRect(&image.native(), skia::from(region), SkRect::MakeXYWH(point.x, point.y, region.x, region.y), nullptr);
    }

    void Graphics::drawImage(const Image & image, core::pt32f point, core::rc32f region, int32_t flags)
    {
        if (!_native)
            return;

        if (flags & core::align::right)
            point.x = point.x - region.cx;
        else if (flags & core::align::centerX)
            point.x = point.x - region.cx / 2;
        else {}

        if (flags & core::align::bottom)
            point.y = point.y - region.cy;
        else if (flags & core::align::centerY)
            point.y = point.y - region.cy / 2;
        else {}

        _native->drawImageRect(&image.native(), skia::from(region), SkRect::MakeXYWH(point.x, point.y, region.x, region.y), nullptr);
    }

    void Graphics::drawImage(const Image & image, core::rc32f rect, core::rc32i region, int32_t flags)
    {
        if (!_native)
            return;

        core::pt32i point;
        if (flags & core::align::right)
            point.x = rect.right() - region.cx;
        else if (flags & core::align::centerX)
            point.x = rect.centerX() - region.cx / 2;
        else
            point.x = rect.x;

        if (flags & core::align::bottom)
            point.y = rect.bottom() - region.cy;
        else if (flags & core::align::centerY)
            point.y = rect.centerY() - region.cy / 2;
        else
            point.y = rect.y;

        _native->drawImage(&image.native(), point.x, point.y, nullptr);
    }

    void Graphics::drawImage(const Image & image, core::rc32f rect, core::rc32f region, int32_t flags)
    {
        if (!_native)
            return;

        core::pt32i point;
        if (flags & core::align::right)
            point.x = rect.right() - region.cx;
        else if (flags & core::align::centerX)
            point.x = rect.centerX() - region.cx / 2;
        else
            point.x = rect.x;

        if (flags & core::align::bottom)
            point.y = rect.bottom() - region.cy;
        else if (flags & core::align::centerY)
            point.y = rect.centerY() - region.cy / 2;
        else
            point.y = rect.y;

        _native->drawImage(&image.native(), point.x, point.y, nullptr);
    }

    void Graphics::drawImage(const Image & image, core::rc32f rect)
    {
        if (!_native)
            return;

        _native->drawImageRect(&image.native(), skia::from(rect), nullptr);
    }

    void Graphics::drawImage(const Image & image, core::rc32f rect, core::rc32i region)
    {
        if (!_native)
            return;

        _native->drawImageRect(&image.native(), skia::from(region), skia::from(rect), nullptr);
    }

    void Graphics::drawImage(const Image & image, core::rc32f rect, core::rc32f region)
    {
        if (!_native)
            return;

        _native->drawImageRect(&image.native(), skia::from(region), skia::from(rect), nullptr);
    }

    void Graphics::FillPath(graphics::raster::path & path, core::color32 color)
    {
        if (!_native)
            return;

        //_native->FillPath(path, color);
    }

    text::fontmetrics Graphics::GetFontMetrics(text::font font)
    {
        if (!_native)
            return {};
        return {};
        //return _native->GetFontMetrics(font);
    }

    core::si32i Graphics::MeasureString(std::string str, text::font font)
    {
        if (!_native)
            return {};

        //return _native->MeasureString(str, font);
    }
}
