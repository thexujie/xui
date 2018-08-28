#pragma once

#include "GraphicsService.h"
#include "TextBlob.h"
#include "Bitmap.h"
#include "Image.h"
#include "PathStyle.h"
#include "StringFormat.h"
#include "Path.h"

class SkCanvas;
class SkTextBlobBuilder;

namespace graphics
{
    struct drawcall_statistics
    {
        int32_t save = 0;
        int32_t restore = 0;
        int32_t setClipRect = 0;
        int32_t setClipPath = 0;
        int32_t setMatrix = 0;
        int32_t drawLine = 0;
        int32_t drawEllipse = 0;
        int32_t drawRectangle = 0;
        int32_t drawRoundRect = 0;
        int32_t drawPath = 0;
        int32_t drawString___const_std_string_ref__core_pt32f_const_StringFormat_ref = 0;
        int32_t drawString___const_std_string_ref__core_rc32f_const_StringFormat_ref = 0;
        int32_t drawTextBlob = 0;
        int32_t drawImage__const_Image_ref__core_pt32f__core_aligns = 0;
        int32_t drawImage__const_Image_ref__core_rc32f__core_aligns = 0;
        int32_t drawImage__const_Image_ref__core_pt32f__core_rc32i_core_aligns = 0;
        int32_t drawImage__const_Image_ref__core_pt32f__core_rc32f_core_aligns = 0;
        int32_t drawImage__const_Image_ref__core_rc32f__core_rc32i_core_aligns = 0;
        int32_t drawImage__const_Image_ref__core_rc32f__core_rc32f_core_aligns = 0;
        int32_t drawImage__const_Image_ref__core_pt32f = 0;
        int32_t drawImage__const_Image_ref__core_rc32f = 0;
        int32_t drawImage__const_Image_ref__core_rc32f__core_rc32i = 0;
        int32_t drawImage__const_Image_ref__core_rc32f__core_rc32f = 0;
        int32_t fillPath = 0;

        int32_t total() const
        {
            return 
                //save +
                //restore +
                setClipRect +
                setClipPath +
                setMatrix +
                drawLine +
                drawEllipse +
                drawRectangle +
                drawRoundRect +
                drawPath +
                drawString___const_std_string_ref__core_pt32f_const_StringFormat_ref +
                drawString___const_std_string_ref__core_rc32f_const_StringFormat_ref +
                drawTextBlob +
                drawImage__const_Image_ref__core_pt32f__core_aligns +
                drawImage__const_Image_ref__core_rc32f__core_aligns +
                drawImage__const_Image_ref__core_pt32f__core_rc32i_core_aligns +
                drawImage__const_Image_ref__core_pt32f__core_rc32f_core_aligns +
                drawImage__const_Image_ref__core_rc32f__core_rc32i_core_aligns +
                drawImage__const_Image_ref__core_rc32f__core_rc32f_core_aligns +
                drawImage__const_Image_ref__core_pt32f +
                drawImage__const_Image_ref__core_rc32f +
                drawImage__const_Image_ref__core_rc32f__core_rc32i +
                drawImage__const_Image_ref__core_rc32f__core_rc32f +
                fillPath;
        }
    };

    class Graphics
    {
    public:
        Graphics() = default;
        ~Graphics() = default;

        //Graphics(core::si32i size);
        Graphics(std::shared_ptr<Bitmap> pixmap);

    public:
        void clear(core::color32 color);
        void save();
        void restore();
        void setClipRect(const core::rc32f & rect, bool aa = true);
        void setClipPath(std::shared_ptr<graphics::Path> path, bool aa = true);
        void setMatrix(const core::float3x2 & matrix);

        void drawLine(core::pt32f start, core::pt32f end, const PathStyle & style);

        void drawEllipse(core::rc32f ellipse, const PathStyle & style);
        void drawRectangle(core::rc32f rect, const PathStyle & style);
        void drawRoundRect(core::rc32f rect, float32_t rx, float32_t ry, const PathStyle & style);

        void drawPath(const std::shared_ptr<graphics::Path> & path, const PathStyle & style);

        void drawString(const std::string & str, core::pt32f point, const StringFormat & format);
        void drawString(const std::string & str, core::rc32f rect, const StringFormat & format);
        void drawTextBlob(const graphics::TextBlob & blob, core::pt32f point);

        void drawImage(const Image & image, core::pt32f point, core::aligns align);
        void drawImage(const Image & image, core::rc32f rect, core::aligns align);
        void drawImage(const Image & image, core::pt32f point, core::rc32i region, core::aligns align);
        void drawImage(const Image & image, core::pt32f point, core::rc32f region, core::aligns align);
        void drawImage(const Image & image, core::rc32f rect, core::rc32i region, core::aligns align);
        void drawImage(const Image & image, core::rc32f rect, core::rc32f region, core::aligns align);

        void drawImage(const Image & image, core::pt32f point);
        void drawImage(const Image & image, core::rc32f rect);
        void drawImage(const Image & image, core::rc32f rect, core::rc32i region);
        void drawImage(const Image & image, core::rc32f rect, core::rc32f region);

    public:
        const drawcall_statistics & statistics() const { return _statistics; }
        void fillPath(graphics::raster::path & path, core::color32 color);
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
        drawcall_statistics _statistics;
    };
}
