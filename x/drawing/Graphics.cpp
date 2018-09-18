#include "stdafx.h"
#include "Graphics.h"
#include "skia/skia.h"
#include "skia/SkShaper.h"
#include <SkScalar.h>
#include <SkTextBlob.h>
#include <SkPathEffect.h>
#include <SkDashPathEffect.h>
#include <SkDiscretePathEffect.h>
#include <Sk2DPathEffect.h>
#include <SkColorFilter.h>

namespace drawing
{
    //Graphics::Graphics(core::si32i size): _native(GraphicsService().CreateGraphics(GraphicsService().CreatePixmap(dimension))) { }

    Graphics::Graphics(std::shared_ptr<Bitmap> pixmap)
    {
        _pixmap = pixmap;
        _native = std::make_shared<SkCanvas>(pixmap->native());
    }

    void Graphics::clear(core::color32 color)
    {
        _statistics = {};
        if (!_native)
            return;

        _native->clear(color);
    }

    void Graphics::save()
    {
        ++_statistics.save;
        if (!_native)
            return;
        _native->save();
    }

    void Graphics::saveLayer(const core::rc32f & bounds, uint8_t alpha)
    {
        ++_statistics.save;
        if (!_native)
            return;

        auto b = skia::from(bounds);
        _native->saveLayerAlpha(&b, alpha);
    }

    void Graphics::restore()
    {
        ++_statistics.restore;
        if (!_native)
            return;

        _native->restore();
    }

    void Graphics::setClipRect(const core::rc32f & rect, bool aa)
    {
        ++_statistics.setClipRect;
        if (!_native)
            return;

        _native->clipRect(skia::from(rect), aa);
    }

    void Graphics::setClipPath(const drawing::Path & path, bool aa)
    {
        ++_statistics.setClipPath;
        if (!_native)
            return;

        _native->clipPath(path.native(), aa);
    }

    void Graphics::setMatrix(const core::float3x2 & matrix)
    {
        ++_statistics.setMatrix;
        if (!_native)
            return;

        _native->setMatrix(skia::from(matrix));
    }

    void Graphics::setColorFilter(std::shared_ptr<ColorFilter> colorFilter)
    {
        _colorFilter = colorFilter;
    }

    void Graphics::drawLine(core::pt32f start, core::pt32f end, const PathStyle & style)
    {
        ++_statistics.drawLine;
        if (!_native)
            return;

        SkPaint paint;
        style.apply(paint);
        apply(paint);
        _native->drawLine({ start.x, start.y }, { end.x, end.y }, paint);
    }

    void Graphics::drawEllipse(core::rc32f ellipse, const PathStyle & style)
    {
        ++_statistics.drawEllipse;
        if (!_native)
            return;

        SkPaint paint;
        style.apply(paint);
        apply(paint);
        _native->drawOval(skia::from(ellipse), paint);
    }

    void Graphics::drawRectangle(core::rc32f rect, const PathStyle & style)
    {
        ++_statistics.drawRectangle;
        if (!_native)
            return;

        SkPaint paint;
        style.apply(paint);
        apply(paint);
        _native->drawRect(skia::from(rect), paint);
    }

    void Graphics::drawRoundRect(core::rc32f rect, float32_t rx, float32_t ry, const PathStyle & style)
    {
        ++_statistics.drawRoundRect;
        if (!_native)
            return;

        SkPaint paint;
        style.apply(paint);
        apply(paint);
        _native->drawRoundRect(skia::from(rect), rx, ry, paint);
    }

    void Graphics::drawPath(const std::shared_ptr<drawing::Path> & path, const PathStyle & style)
    {
        ++_statistics.drawPath;
        if (!_native)
            return;

        SkPaint paint;
        {
            SkScalar scale = 10.0f;
            SkPath path;
            static const int8_t pts[] = { 2, 2, 1, 3, 0, 3, 2, 1, 3, 1,
                4, 0, 4, 1, 5, 1, 4, 2, 4, 3, 2, 5, 2, 4, 3, 3, 2, 3 };
            path.moveTo(2 * scale, 3 * scale);
            for (size_t i = 0; i < sizeof(pts) / sizeof(pts[0]); i += 2)
            {
                path.lineTo(pts[i] * scale, pts[i + 1] * scale);
            }
            path.close();
            SkMatrix matrix = SkMatrix::MakeScale(4 * scale);
            SkPaint paint;
            paint.setPathEffect(SkPath2DPathEffect::Make(matrix, path));
            paint.setAntiAlias(true);
        }
        style.apply(paint);
        apply(paint);
        _native->drawPath(path->native(), paint);
    }

    void Graphics::drawString(const std::string & str, core::pt32f point, const StringFormat & format)
    {
        ++_statistics.drawString___const_std_string_ref__core_pt32f_const_StringFormat_ref;
        if (!_native)
            return;

        SkPaint paint;
        format.apply(paint);
        apply(paint);
        TextClusterizer shaper;
        shaper.itermize(str, format._font, format._color);
        shaper.layout();
        core::si32f size = shaper.bounds();
        auto blob = shaper.build();

        if (format._align & core::align::right)
            point.x -= size.cx;
        else if (format._align & core::align::centerX)
            point.x -= size.cx / 2;
        else {}

        if (format._align & core::align::bottom)
            point.y -= size.cy;
        else if (format._align & core::align::centerY)
            point.y -= size.cy / 2;
        else {}

        _native->drawTextBlob(blob.get(), point.x, point.y, paint);
    }

    void Graphics::drawString(const std::string & str, core::rc32f rect, const StringFormat & format)
    {
        ++_statistics.drawString___const_std_string_ref__core_rc32f_const_StringFormat_ref;
        if (!_native)
            return;

        SkPaint paint;
        format.apply(paint);
        apply(paint);

        TextClusterizer shaper;
        shaper.itermize(str, format._font, format._color);
        shaper.layout();
        core::si32f size = shaper.bounds();
        auto blob = shaper.build();

        core::pt32f point = rect.leftTop();
        if (format._align & core::align::right)
            point.x = rect.right() - size.cx;
        else if (format._align & core::align::centerX)
            point.x = (rect.cx - size.cx) / 2;
        else {}

        if (format._align & core::align::bottom)
            point.y = rect.bottom() - size.cy;
        else if (format._align & core::align::centerY)
            point.y = (rect.cy - size.cy) / 2;
        else {}

        _native->clipRect(skia::from(rect));
        _native->drawTextBlob(blob.get(), point.x, point.y, paint);
        _native->clipRect(SkRect());
    }

    void Graphics::drawTextBlob(const drawing::TextBlob & blob, core::pt32f point)
    {
        ++_statistics.drawTextBlob;
        if (!_native)
            return;

        SkPaint paint;
        apply(paint);

        _native->drawTextBlob(blob.native_ptr(), point.x, point.y, paint);
    }

    void Graphics::drawImage(const Image & image, core::pt32f point, core::aligns align)
    {
        ++_statistics.drawImage__const_Image_ref__core_pt32f__core_aligns;
        if (!_native)
            return;

        auto size = image.size();
        if (align & core::align::right)
            point.x -= size.cx;
        else if (align & core::align::centerX)
            point.x -= size.cx / 2;
        else {}

        if (align & core::align::bottom)
            point.y -= size.cy;
        else if (align & core::align::centerY)
            point.y -= size.cy / 2;
        else {}

        SkPaint paint;
        apply(paint);
        _native->drawImage(&image.native(), point.x, point.y, &paint);
    }

    void Graphics::drawImage(const Image & image, core::rc32f rect, core::aligns align)
    {
        ++_statistics.drawImage__const_Image_ref__core_rc32f__core_aligns;
        if (!_native)
            return;

        core::pt32f point;
        auto size = image.size();
        if (align & core::align::right)
            point.x = rect.right() - size.cx;
        else if (align & core::align::centerX)
            point.x = rect.centerX() - size.cx / 2;
        else
            point.x = rect.x;

        if (align & core::align::bottom)
            point.y = rect.bottom() - size.cy;
        else if (align & core::align::centerY)
            point.y = rect.centerY() - size.cy / 2;
        else
            point.y = rect.y;

        SkPaint paint;
        apply(paint);
        _native->drawImage(&image.native(), point.x, point.y, &paint);
    }

    void Graphics::drawImage(const Image & image, core::pt32f point, core::rc32i region, core::aligns align)
    {
        ++_statistics.drawImage__const_Image_ref__core_pt32f__core_rc32i_core_aligns;
        if (!_native)
            return;

        if (align & core::align::right)
            point.x = point.x - region.cx;
        else if (align & core::align::centerX)
            point.x = point.x - region.cx / 2;
        else {}

        if (align & core::align::bottom)
            point.y = point.y - region.cy;
        else if (align & core::align::centerY)
            point.y = point.y - region.cy / 2;
        else {}

        SkPaint paint;
        apply(paint);
        _native->drawImageRect(&image.native(), skia::from(region), SkRect::MakeXYWH(point.x, point.y, float32_t(region.x), float32_t(region.y)), &paint);
    }

    void Graphics::drawImage(const Image & image, core::pt32f point, core::rc32f region, core::aligns align)
    {
        ++_statistics.drawImage__const_Image_ref__core_pt32f__core_rc32f_core_aligns;
        if (!_native)
            return;

        if (align & core::align::right)
            point.x = point.x - region.cx;
        else if (align & core::align::centerX)
            point.x = point.x - region.cx / 2;
        else {}

        if (align & core::align::bottom)
            point.y = point.y - region.cy;
        else if (align & core::align::centerY)
            point.y = point.y - region.cy / 2;
        else {}

        SkPaint paint;
        apply(paint);
        _native->drawImageRect(&image.native(), skia::from(region), SkRect::MakeXYWH(point.x, point.y, region.x, region.y), &paint);
    }

    void Graphics::drawImage(const Image & image, core::rc32f rect, core::rc32i region, core::aligns align)
    {
        ++_statistics.drawImage__const_Image_ref__core_rc32f__core_rc32i_core_aligns;
        if (!_native)
            return;

        core::pt32f point;
        if (align & core::align::right)
            point.x = rect.right() - region.cx;
        else if (align & core::align::centerX)
            point.x = rect.centerX() - region.cx / 2;
        else
            point.x = rect.x;

        if (align & core::align::bottom)
            point.y = rect.bottom() - region.cy;
        else if (align & core::align::centerY)
            point.y = rect.centerY() - region.cy / 2;
        else
            point.y = rect.y;

        SkPaint paint;
        apply(paint);
        _native->drawImage(&image.native(), point.x, point.y, &paint);
    }

    void Graphics::drawImage(const Image & image, core::rc32f rect, core::rc32f region, core::aligns align)
    {
        ++_statistics.drawImage__const_Image_ref__core_rc32f__core_rc32f_core_aligns;
        if (!_native)
            return;

        core::pt32f point;
        if (align & core::align::right)
            point.x = rect.right() - region.cx;
        else if (align & core::align::centerX)
            point.x = rect.centerX() - region.cx / 2;
        else
            point.x = rect.x;

        if (align & core::align::bottom)
            point.y = rect.bottom() - region.cy;
        else if (align & core::align::centerY)
            point.y = rect.centerY() - region.cy / 2;
        else
            point.y = rect.y;

        SkPaint paint;
        apply(paint);
        _native->drawImage(&image.native(), point.x, point.y, &paint);
    }

    void Graphics::drawImage(const Image & image, core::pt32f point)
    {
        ++_statistics.drawImage__const_Image_ref__core_pt32f;
        if (!_native)
            return;

        SkPaint paint;
        apply(paint);
        _native->drawImage(&image.native(), point.x, point.y, &paint);
    }

    void Graphics::drawImage(const Image & image, core::rc32f rect)
    {
        ++_statistics.drawImage__const_Image_ref__core_rc32f;
        if (!_native)
            return;

        SkPaint paint;
        apply(paint);
        _native->drawImageRect(&image.native(), skia::from(rect), &paint);
    }

    void Graphics::drawImage(const Image & image, core::rc32f rect, core::rc32i region)
    {
        ++_statistics.drawImage__const_Image_ref__core_rc32f__core_rc32i;
        if (!_native)
            return;

        SkPaint paint;
        apply(paint);
        _native->drawImageRect(&image.native(), skia::from(region), skia::from(rect), &paint);
    }

    void Graphics::drawImage(const Image & image, core::rc32f rect, core::rc32f region)
    {
        ++_statistics.drawImage__const_Image_ref__core_rc32f__core_rc32f;
        if (!_native)
            return;

        SkPaint paint;
        apply(paint);
        _native->drawImageRect(&image.native(), skia::from(region), skia::from(rect), &paint);
    }

    void Graphics::fillPath(path_source & path, core::color32 color)
    {
        ++_statistics.fillPath;
        if (!_native)
            return;

        SkPaint paint;
        apply(paint);
        //_native->fillPath(path, color);
    }

    fontmetrics Graphics::GetFontMetrics(font font)
    {
        if (!_native)
            return {};
        return {};
        //return _native->GetFontMetrics(font);
    }

    core::si32i Graphics::MeasureString(std::string str, font font)
    {
        if (!_native)
            return {};

        //return _native->MeasureString(str, font);
    }

    void Graphics::apply(SkPaint & paint) const
    {
        if (_colorFilter)
            paint.setColorFilter(sk_ref_sp(_colorFilter->native_ptr()));
    }
}
