#pragma once

class SkCanvas;
class SkTextBlobBuilder;

namespace drawing
{
    enum class point_mode
    {
        points,
        lines,
        polygon,
    };

    struct drawcall_statistics
    {
        uint32_t save = 0;
        uint32_t restore = 0;
        uint32_t setClipRect = 0;
        uint32_t setClipPath = 0;
        uint32_t setMatrix = 0;
        uint32_t drawLine = 0;
        uint32_t drawEllipse = 0;
        uint32_t drawRectangle = 0;
        uint32_t drawRoundRect = 0;
        uint32_t drawPoints = 0;
        uint32_t drawPath = 0;
        uint32_t drawString___const_std_string_ref__core_pt32f_const_StringFormat_ref = 0;
        uint32_t drawString___const_std_string_ref__core_rc32f_const_StringFormat_ref = 0;
        uint32_t drawTextBlob = 0;
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
        Graphics(std::shared_ptr<Surface> pixmap);

    public:
        void clear(core::color32 color);
        void save();
        void saveLayer(const core::rc32f & bounds, uint8_t alpha);
        void restore();
        void setClipRect(const core::rc32f & rect, bool aa = true);
        void setClipPath(const drawing::Path & path, bool aa = true);
        void setMatrix(const core::float3x2 & matrix);
        void setColorFilter(std::shared_ptr<ColorFilter> colorFilter);

        void drawLine(core::pt32f start, core::pt32f end, const PathStyle & style);

        void drawEllipse(const core::rc32f & ellipse, const PathStyle & style);
        void drawRectangle(const core::rc32f & rect, const PathStyle & style);
        void drawRoundRect(const core::rc32f & rect, float32_t rx, float32_t ry, const PathStyle & style);

        void drawPoints(const core::pt32f points[], size_t count, point_mode mode, const PathStyle & style);
        void drawPath(const Path & path, const PathStyle & style);
        void drawPath(const std::shared_ptr<drawing::Path> & path, const PathStyle & style);

        void drawString(const std::string & str, const core::pt32f & pos, const StringFormat & format);
        void drawString(const std::string & str, const core::rc32f & rect, const StringFormat & format);
        void drawTextBlob(const drawing::TextBlob & blob, core::pt32f point);

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
        void fillPath(path_source & path, core::color32 color);
        fontmetrics GetFontMetrics(font font);
        core::si32i MeasureString(std::string str, font font);

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
        drawcall_statistics _statistics;

        std::shared_ptr<ColorFilter> _colorFilter;
    };
}
