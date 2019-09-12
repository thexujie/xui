#include "stdafx.h"
#include "Graphics.h"

#include "skia/skia.h"
#include <SkScalar.h>
#include <SkTextBlob.h>
#include <SkPathEffect.h>
#include <SkDashPathEffect.h>
#include <SkDiscretePathEffect.h>
#include <Sk2DPathEffect.h>
#include <SkColorFilter.h>
#include <SkDrawable.h>

#include "Bitmap.h"
#include "Image.h"
#include "Path.h"
#include "ColorFilter.h"
#include "Text.h"

namespace drawing::skia
{
    //Graphics::Graphics(core::sizei size): _native(GraphicsService().CreateGraphics(GraphicsService().CreatePixmap(dimension))) { }

    Graphics::Graphics(std::shared_ptr<skia::Bitmap> pixmap)
    {
        _pixmap = pixmap;
        _native = std::make_shared<SkCanvas>(pixmap->native());
    }

    Graphics::Graphics(std::shared_ptr<skia::Surface> surface)
    {
        _surface = surface;
        _native = std::shared_ptr<SkCanvas>(surface->native().getCanvas(), [](SkCanvas *){});
    }

    Graphics::~Graphics()
    {
        assert(_statistics.save == _statistics.restore);
    }

    void Graphics::clear(core::color color)
    {
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

    void Graphics::saveLayer(const core::rectf & bounds, uint8_t alpha)
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

    void Graphics::setClipRect(const core::rectf & rect, bool aa)
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

        _native->clipPath(((skia::Path &)path).native(), aa);
    }

    void Graphics::setMatrix(const core::float3x2 & matrix)
    {
        ++_statistics.setMatrix;
        if (!_native)
            return;

        _native->setMatrix(skia::from(matrix));
    }

    void Graphics::resetMatrix()
    {
        ++_statistics.resetMatrix;
        if (!_native)
            return;

        _native->resetMatrix();
    }

    void Graphics::setColorFilter(const drawing::ColorFilter & colorFilter)
    {
        _colorFilter = ((skia::ColorFilter &)colorFilter).native_shared();
    }

    void Graphics::drawLine(core::pointf start, core::pointf end, const drawing::PathFormat & format)
    {
        ++_statistics.drawLine;
        if (!_native)
            return;

        SkPaint paint;
        apply(paint);
		apply_pathformat(paint, format);
        _native->drawLine({ start.x, start.y }, { end.x, end.y }, paint);
    }

    void Graphics::drawEllipse(const core::rectf & ellipse, const drawing::PathFormat & format)
    {
        ++_statistics.drawEllipse;
        if (!_native)
            return;

        SkPaint paint;
        apply(paint);
		apply_pathformat(paint, format);
        _native->drawOval(skia::from(ellipse), paint);
    }

    void Graphics::drawRectangle(const core::rectf & rect, const drawing::PathFormat & format)
    {
        ++_statistics.drawRectangle;
        if (!_native)
            return;

        SkPaint paint;
        apply(paint);
		apply_pathformat(paint, format);
        _native->drawRect(skia::from(rect), paint);
    }

    void Graphics::drawRoundRect(const core::rectf & rect, float32_t rx, float32_t ry, const drawing::PathFormat & format)
    {
        ++_statistics.drawRoundRect;
        if (!_native)
            return;

        SkPaint paint;
        apply(paint);
		apply_pathformat(paint, format);
        _native->drawRoundRect(skia::from(rect), rx, ry, paint);
    }

    void Graphics::drawPoints(const core::pointf points[], size_t count, drawing::point_mode mode, const drawing::PathFormat & format)
    {
        ++_statistics.drawPoints;
        if (!_native)
            return;

        SkPaint paint;
        apply(paint);
		apply_pathformat(paint, format);
        _native->drawPoints(skia::from(mode), count, reinterpret_cast<const SkPoint *>(points), paint);
    }

    void Graphics::drawPath(const drawing::Path & path, const drawing::PathFormat & format)
    {
        ++_statistics.drawPath;
        if (!_native)
            return;

        SkPaint paint;
        apply(paint);
		apply_pathformat(paint, format);
        _native->drawPath(((skia::Path &)path).native(), paint);
    }

    void Graphics::drawPath(const std::shared_ptr<drawing::Path> & path, const drawing::PathFormat & format)
    {
        ++_statistics.drawPath;
        if (!_native)
            return;

        SkPaint paint;
        ////{
        ////    SkScalar scale = 10.0f;
        ////    SkPath path;
        ////    static const int8_t pts[] = { 2, 2, 1, 3, 0, 3, 2, 1, 3, 1,
        ////        4, 0, 4, 1, 5, 1, 4, 2, 4, 3, 2, 5, 2, 4, 3, 3, 2, 3 };
        ////    path.moveTo(2 * scale, 3 * scale);
        ////    for (size_t i = 0; i < sizeof(pts) / sizeof(pts[0]); i += 2)
        ////    {
        ////        path.lineTo(pts[i] * scale, pts[i + 1] * scale);
        ////    }
        ////    path.close();
        ////    SkMatrix matrix = SkMatrix::MakeScale(4 * scale);
        ////    SkPaint paint;
        ////    paint.setPathEffect(SkPath2DPathEffect::Make(matrix, path));
        ////    paint.setAntiAlias(true);
        ////}
        apply(paint);
		apply_pathformat(paint, format);
        _native->drawPath(((skia::Path &)path).native(), paint);
    }

    void Graphics::drawString(const std::u8string & str, const core::pointf & pos, const drawing::StringFormat & format)
    {
        ++_statistics.drawString___const_std_string_ref__core_pt32f_const_StringFormat_ref;
        if (!_native)
            return;

        SkPaint paint;
        apply(paint);
		apply_stringformat(paint, format);
    	
        skia::Text shaper(str);
        shaper.itermize(format.font, format.color);
        shaper.layout();
        core::sizef size = shaper.bounds();
        auto blob = shaper.build();

        core::pointf point = pos;
        if (format.align & core::align::right)
            point.x -= size.cx;
        else if (format.align & core::align::centerX)
            point.x -= size.cx / 2;
        else {}

        if (format.align & core::align::bottom)
            point.y -= size.cy;
        else if (format.align & core::align::centerY)
            point.y -= size.cy / 2;
        else {}

        _native->drawTextBlob(blob.get(), point.x, point.y, paint);
    }

    void Graphics::drawString(const std::u8string & str, const core::rectf & rect, const drawing::StringFormat & format)
    {
        ++_statistics.drawString___const_std_string_ref__core_rc32f_const_StringFormat_ref;
        if (!_native)
            return;

        SkPaint paint;
        apply(paint);
		apply_stringformat(paint, format);

        Text shaper(str);
        shaper.itermize(format.font, format.color);
        shaper.layout();
        core::sizef size = shaper.bounds();
        auto blob = shaper.build(rect.cx);

        core::pointf point = rect.leftTop();
        if (format.align & core::align::right)
            point.x = rect.right() - size.cx;
        else if (format.align & core::align::centerX)
            point.x = (rect.cx - size.cx) / 2;
        else {}

        if (format.align & core::align::bottom)
            point.y = rect.bottom() - size.cy;
        else if (format.align & core::align::centerY)
            point.y = (rect.cy - size.cy) / 2;
        else {}

        _native->save();
        _native->clipRect(skia::from(rect));
        _native->drawTextBlob(blob.get(), point.x, point.y, paint);
        _native->clipRect(SkRect());
        _native->restore();
    }

    void Graphics::drawText(const drawing::Text & text, core::pointf point, const drawing::StringFormat & format)
    {
        ++_statistics.drawText;
        if (!_native)
            return;

        SkPaint paint;
        apply(paint);
		apply_stringformat(paint, format);

        _native->drawTextBlob( ((skia::Text &)text).blob().get(), point.x, point.y, paint);
    }

    void Graphics::drawImage(const drawing::Image & image, core::pointf point, core::aligns align, const drawing::ImageFormat & format)
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
		apply_imageformat(paint, format);
        _native->drawImage(((skia::Image &)image).native_ptr(), point.x, point.y, &paint);
    }

    void Graphics::drawImage(const drawing::Image & image, core::rectf rect, core::aligns align, const drawing::ImageFormat & format)
    {
        ++_statistics.drawImage__const_Image_ref__core_rc32f__core_aligns;
        if (!_native)
            return;

        core::pointf point;
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
		apply_imageformat(paint, format);
        _native->drawImage(((skia::Image &)image).native_ptr(), point.x, point.y, &paint);
    }

    void Graphics::drawImage(const drawing::Image & image, core::pointf point, core::recti region, core::aligns align, const drawing::ImageFormat & format)
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
		apply_imageformat(paint, format);
        _native->drawImageRect(((skia::Image &)image).native_ptr(), skia::from(region), SkRect::MakeXYWH(point.x, point.y, float32_t(region.x), float32_t(region.y)), &paint);
    }

    void Graphics::drawImage(const drawing::Image & image, core::pointf point, core::rectf region, core::aligns align, const drawing::ImageFormat & format)
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
		apply_imageformat(paint, format);
        _native->drawImageRect(((skia::Image &)image).native_ptr(), skia::from(region), SkRect::MakeXYWH(point.x, point.y, region.x, region.y), &paint);
    }

    void Graphics::drawImage(const drawing::Image & image, core::rectf rect, core::recti region, core::aligns align, const drawing::ImageFormat & format)
    {
        ++_statistics.drawImage__const_Image_ref__core_rc32f__core_rc32i_core_aligns;
        if (!_native)
            return;

        core::pointf point;
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
		apply_imageformat(paint, format);
        _native->drawImage(((skia::Image &)image).native_ptr(), point.x, point.y, &paint);
    }

    void Graphics::drawImage(const drawing::Image & image, core::rectf rect, core::rectf region, core::aligns align, const drawing::ImageFormat & format)
    {
        ++_statistics.drawImage__const_Image_ref__core_rc32f__core_rc32f_core_aligns;
        if (!_native)
            return;

        core::pointf point;
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
		apply_imageformat(paint, format);
        _native->drawImage(((skia::Image &)image).native_ptr(), point.x, point.y, &paint);
    }

    void Graphics::drawImage(const drawing::Image & image, core::pointf point, const drawing::ImageFormat & format)
    {
        ++_statistics.drawImage__const_Image_ref__core_pt32f;
        if (!_native)
            return;

        SkPaint paint;
        apply(paint);
		apply_imageformat(paint, format);
        _native->drawImage(((skia::Image &)image).native_ptr(), point.x, point.y, &paint);
    }

    void Graphics::drawImage(const drawing::Image & image, core::rectf rect, const drawing::ImageFormat & format)
    {
        ++_statistics.drawImage__const_Image_ref__core_rc32f;
        if (!_native)
            return;

        SkPaint paint;
        apply(paint);
		apply_imageformat(paint, format);
        _native->drawImageRect(((skia::Image &)image).native_ptr(), skia::from(rect), &paint);
    }

    void Graphics::drawImage(const drawing::Image & image, core::rectf rect, core::recti region, const drawing::ImageFormat & format)
    {
        ++_statistics.drawImage__const_Image_ref__core_rc32f__core_rc32i;
        if (!_native)
            return;

        SkPaint paint;
        apply(paint);
		apply_imageformat(paint, format);
        _native->drawImageRect(((skia::Image &)image).native_ptr(), skia::from(region), skia::from(rect), &paint);
    }

    void Graphics::drawImage(const drawing::Image & image, core::rectf rect, core::rectf region, const drawing::ImageFormat & format)
    {
        ++_statistics.drawImage__const_Image_ref__core_rc32f__core_rc32f;
        if (!_native)
            return;

        SkPaint paint;
        apply(paint);
		apply_imageformat(paint, format);
        _native->drawImageRect(((skia::Image &)image).native_ptr(), skia::from(region), skia::from(rect), &paint);
    }

    void Graphics::fillPath(drawing::path_source & path, core::color color)
    {
        ++_statistics.fillPath;
        if (!_native)
            return;

        SkPaint paint;
        apply(paint);
        //_native->fillPath(path, color);
    }

    void Graphics::apply(SkPaint & paint) const
    {
        if (_colorFilter)
            paint.setColorFilter(sk_ref_sp(_colorFilter.get()));
    }
}
