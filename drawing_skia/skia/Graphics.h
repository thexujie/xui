#pragma once

#include "skia.h"

class SkColorFilter;
class SkPaint;
class SkCanvas;
class SkTextBlobBuilder;

namespace drawing::skia
{
	class Bitmap;
	class Surface;

	struct drawcall_statistics
    {
        uint32_t save = 0;
        uint32_t restore = 0;
        uint32_t setClipRect = 0;
        uint32_t setClipRegion = 0;
        uint32_t setClipPath = 0;
        uint32_t setMatrix = 0;
        uint32_t resetMatrix = 0;
        uint32_t drawLine = 0;
        uint32_t drawEllipse = 0;
        uint32_t drawRectangle = 0;
        uint32_t drawRoundRect = 0;
        uint32_t drawPoints = 0;
        uint32_t drawPath = 0;
        uint32_t drawString___const_std_string_ref__core_pt32f_const_StringFormat_ref = 0;
        uint32_t drawString___const_std_string_ref__core_rc32f_const_StringFormat_ref = 0;
        uint32_t drawTextBlob = 0;
        uint32_t drawText = 0;
        uint32_t drawImage__const_Image_ref__core_pt32f__core_aligns = 0;
        uint32_t drawImage__const_Image_ref__core_rc32f__core_aligns = 0;
        uint32_t drawImage__const_Image_ref__core_pt32f__core_rc32i_core_aligns = 0;
        uint32_t drawImage__const_Image_ref__core_pt32f__core_rc32f_core_aligns = 0;
        uint32_t drawImage__const_Image_ref__core_rc32f__core_rc32i_core_aligns = 0;
        uint32_t drawImage__const_Image_ref__core_rc32f__core_rc32f_core_aligns = 0;
        uint32_t drawImage__const_Image_ref__core_pt32f = 0;
        uint32_t drawImage__const_Image_ref__core_rc32f = 0;
        uint32_t drawImage__const_Image_ref__core_rc32f__core_rc32i = 0;
        uint32_t drawImage__const_Image_ref__core_rc32f__core_rc32f = 0;
        uint32_t fillPath = 0;

        uint32_t total() const
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
                drawText +
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

    class Graphics : public drawing::Graphics
    {
    public:
        Graphics() = default;

        //Graphics(core::sizei size);
        Graphics(std::shared_ptr<skia::Bitmap> pixmap);
        Graphics(std::shared_ptr<skia::Surface> pixmap);
        ~Graphics();

    public:
        void clear(core::color color);
        void save();
        void saveLayer(const core::rectf & bounds, uint8_t alpha);
        void restore();
        void setClipRect(const core::rectf & rect, bool aa = true);
        void setClipPath(const drawing::Path & path, bool aa = true);
        void setMatrix(const core::float3x2 & matrix);
        void resetMatrix();
        void setColorFilter(const drawing::ColorFilter & colorFilter);

        void drawLine(core::pointf start, core::pointf end, const drawing::PathFormat & format);

        void drawEllipse(const core::rectf & ellipse, const drawing::PathFormat & format);
        void drawRectangle(const core::rectf & rect, const drawing::PathFormat & format);
        void drawRoundRect(const core::rectf & rect, float32_t rx, float32_t ry, const drawing::PathFormat & format);

        void drawPoints(const core::pointf points[], size_t count, drawing::point_mode mode, const drawing::PathFormat & style);
        void drawPath(const drawing::Path & path, const drawing::PathFormat & style);
        void drawPath(const std::shared_ptr<drawing::Path> & path, const drawing::PathFormat & format);

        void drawString(const std::u8string & str, const core::pointf & pos, const drawing::StringFormat & format);
        void drawString(const std::u8string & str, const core::rectf & rect, const drawing::StringFormat & format);
        void drawText(const drawing::Text & text, core::pointf point, const drawing::StringFormat & format);

        void drawImage(const drawing::Image & image, core::pointf point, core::aligns align, const drawing::ImageFormat & format = {});
        void drawImage(const drawing::Image & image, core::rectf rect, core::aligns align, const drawing::ImageFormat & format = {});
        void drawImage(const drawing::Image & image, core::pointf point, core::recti region, core::aligns align, const drawing::ImageFormat & format = {});
        void drawImage(const drawing::Image & image, core::pointf point, core::rectf region, core::aligns align, const drawing::ImageFormat & format = {});
        void drawImage(const drawing::Image & image, core::rectf rect, core::recti region, core::aligns align, const drawing::ImageFormat & format = {});
        void drawImage(const drawing::Image & image, core::rectf rect, core::rectf region, core::aligns align, const drawing::ImageFormat & format = {});

        void drawImage(const drawing::Image & image, core::pointf point, const drawing::ImageFormat & format = {});
        void drawImage(const drawing::Image & image, core::rectf rect, const drawing::ImageFormat & format = {});
        void drawImage(const drawing::Image & image, core::rectf rect, core::recti region, const drawing::ImageFormat & format = {});
        void drawImage(const drawing::Image & image, core::rectf rect, core::rectf region, const drawing::ImageFormat & format = {});

    public:
        const drawcall_statistics & statistics() const { return _statistics; }
        void fillPath(drawing::path_source & path, core::color color);

    public:
        std::shared_ptr<SkCanvas> native_shared() { return _native; }
        SkCanvas & native() { return *_native; }
        const SkCanvas & native() const { return *_native; }
        SkCanvas * native_ptr() { return _native.get(); }
        const SkCanvas * native_ptr() const { return _native.get(); }

    private:
        void apply(SkPaint & paint) const;

    private:
        std::shared_ptr<Bitmap> _pixmap;
        std::shared_ptr<Surface> _surface;
        std::shared_ptr<SkCanvas> _native;
        std::shared_ptr<SkTextBlobBuilder> _blobBuilder;
		std::shared_ptr<SkColorFilter> _colorFilter;
    	
        drawcall_statistics _statistics;
    };
}
