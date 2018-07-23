#pragma once

#include "GraphicsService.h"
#include "TextBlob.h"
#include "Bitmap.h"
#include "Image.h"
#include "PathStyle.h"
#include "StringFormat.h"

class SkCanvas;
class SkTextBlobBuilder;

namespace graphics
{
    class Graphics
    {
    public:
        Graphics() = default;
        ~Graphics() = default;

        //Graphics(core::si32i size);
        Graphics(std::shared_ptr<Bitmap> pixmap);

    public:
        void Clear(core::color32 color);
        void drawLine(core::pt32f start, core::pt32f end, const PathStyle & style);

        void drawEllipse(core::rc32f ellipse, const PathStyle & style);
        void drawRectangle(core::rc32f rect, const PathStyle & style);
        void drawRoundRect(core::rc32f rect, float32_t rx, float32_t ry, const PathStyle & style);

        void drawString(const std::string & str, core::pt32f point, const StringFormat & format);
        void drawString(const std::string & str, core::rc32f rect, const StringFormat & format);
        void drawTextBlob(const graphics::TextBlob & blob, core::pt32f point);

        void drawImage(const Image & image, core::pt32f point, int32_t flags);
        void drawImage(const Image & image, core::rc32f rect, int32_t flags);
        void drawImage(const Image & image, core::pt32f point, core::rc32i region, int32_t flags);
        void drawImage(const Image & image, core::pt32f point, core::rc32f region, int32_t flags);
        void drawImage(const Image & image, core::rc32f rect, core::rc32i region, int32_t flags);
        void drawImage(const Image & image, core::rc32f rect, core::rc32f region, int32_t flags);

        void drawImage(const Image & image, core::rc32f rect);
        void drawImage(const Image & image, core::rc32f rect, core::rc32i region);
        void drawImage(const Image & image, core::rc32f rect, core::rc32f region);

    public:
        void FillPath(graphics::raster::path & path, core::color32 color);
        fontmetrics GetFontMetrics(font font);
        core::si32i MeasureString(std::string str, font font);

    public:
        std::shared_ptr<SkCanvas> native_shared() { return _native; }
        SkCanvas & native() { return *_native; }
        const SkCanvas & native() const { return *_native; }
        SkCanvas * native_ptr() { return _native.get(); }
        const SkCanvas * native_ptr() const { return _native.get(); }

    private:
        std::shared_ptr<Bitmap> _pixmap;
        std::shared_ptr<SkCanvas> _native;
        std::shared_ptr<SkTextBlobBuilder> _blobBuilder;
    };
}
