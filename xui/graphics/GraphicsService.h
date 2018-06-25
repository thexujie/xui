#pragma once

#include "core/vec2.h"
#include "core/rect.h"
#include "image/image.h"
#include "text/text.h"
#include "raster/path.h"

namespace graphics
{
    struct pixmap_buffer
    {
        byte_t * data = nullptr;
        core::si32i size;
        int32_t strike = 0;
        int32_t pitch = 0;
        bool flip_y = false;
    };

    class IGraphicsItem : public core::Object
    {
    public:
        virtual ~IGraphicsItem() = default;
    };

    class IGraphicsPixmap : public IGraphicsItem
    {
    public:
        virtual ~IGraphicsPixmap() = default;
        virtual pixmap_buffer buffer() const = 0;
        virtual core::si32i size() const = 0;
        virtual core::error_e Save(std::string path) const = 0;
    };

    class IGraphicsImage : public IGraphicsItem
    {
    public:
        virtual ~IGraphicsImage() = default;
        virtual core::si32i size() const = 0;
        virtual image::format cmode() const = 0;
        virtual const image::image_data_t & data() const = 0;
        virtual core::error_e Save(std::string path) const = 0;
    };

    class IGraphicsString : public IGraphicsItem
    {
    public:
        virtual ~IGraphicsString() = default;
    };

    class IGraphics : public core::Object
    {
    public:
        virtual ~IGraphics() = default;

        virtual void PushOrign(core::pt32i point) = 0;
        virtual core::pt32i GetOrign() const = 0;
        virtual void PopOrign() = 0;
        virtual void PushClip(core::rc32i rect) = 0;
        virtual core::rc32i GetClip() const = 0;
        virtual void PopClip() = 0;

        virtual void Clear(core::color32 color) = 0;

        virtual void DrawString(std::string str, core::pt32i point, graphics::text::font font, core::color32 color) = 0;

        virtual void DrawImage(const graphics::IGraphicsImage & image, core::pt32i point) = 0;
        virtual void DrawImage(const graphics::IGraphicsImage & image, core::pt32i point, core::rc32i region) = 0;
        virtual void DrawImage(const graphics::IGraphicsImage & image, core::rc32i rect) = 0;
        virtual void DrawImage(const graphics::IGraphicsImage & image, core::rc32i rect, core::rc32i region) = 0;


        virtual void DrawString(const std::string & str, core::pt32i point, const text::font & font, core::color32 color, int32_t flags) = 0;
        virtual void DrawString(const std::string & str, core::rc32i rect, const text::font & font, core::color32 color, int32_t flags) = 0;
        //void DrawString(std::string str, rc32i rect, text::font font, color32 color, int32_t flags);
        //void DrawString(IGraphicsString & str, pt32i point, text::font font, color32 color, int32_t flags);
        //void DrawString(IGraphicsString & str, pt32i point);

        virtual void DrawString(graphics::IGraphicsString & str, core::pt32i point) = 0;

        virtual void FillPath(graphics::raster::path & path, core::color32 color) = 0;

        virtual graphics::text::fontmetrics GetFontMetrics(graphics::text::font font) = 0;
        virtual core::si32i MeasureString(std::string str, graphics::text::font font) = 0;
    };

    class IGraphicsService : public core::Object
    {
    public:
        virtual ~IGraphicsService() = default;
        virtual std::shared_ptr<graphics::IGraphicsPixmap> CreatePixmap(core::si32i) = 0;
        virtual std::shared_ptr<graphics::IGraphicsImage> CreateString(std::string str, graphics::text::font font, core::color32 color) = 0;
        virtual std::shared_ptr<graphics::IGraphics> CreateGraphics(std::shared_ptr<core::Object> pixmap) = 0;
        virtual text::fontmetrics FontMetrics(const text::font & font) = 0;
    };

    IGraphicsService & GraphicsService();
}
