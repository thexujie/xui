#pragma once

#include "core/vec2.h"
#include "core/rect.h"
#include "image/image.h"
#include "raster/path.h"
#include "font.h"

namespace graphics
{
    struct bitmap_buffer
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
        virtual bitmap_buffer buffer() const = 0;
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

        virtual void DrawString(std::string str, core::pt32i point, graphics::font font, core::color32 color) = 0;

        virtual void DrawImage(const graphics::IGraphicsImage & image, core::pt32i point) = 0;
        virtual void DrawImage(const graphics::IGraphicsImage & image, core::pt32i point, core::rc32i region) = 0;
        virtual void DrawImage(const graphics::IGraphicsImage & image, core::rc32i rect) = 0;
        virtual void DrawImage(const graphics::IGraphicsImage & image, core::rc32i rect, core::rc32i region) = 0;


        virtual void DrawString(const std::string & str, core::pt32i point, const font & font, core::color32 color, int32_t flags) = 0;
        virtual void DrawString(const std::string & str, core::rc32i rect, const font & font, core::color32 color, int32_t flags) = 0;
        //void DrawString(std::string str, rc32i rect, text::font font, color32 color, int32_t flags);
        virtual void DrawString(const graphics::IGraphicsString & str, core::pt32i point) = 0;

        virtual void FillPath(graphics::raster::path & path, core::color32 color) = 0;

        virtual graphics::fontmetrics GetFontMetrics(graphics::font font) = 0;
        virtual core::si32i MeasureString(std::string str, graphics::font font) = 0;
    };

    class IGraphicsService : public core::Object
    {
    public:
        virtual ~IGraphicsService() = default;
        virtual fontmetrics FontMetrics(const font & font) = 0;
    };

    IGraphicsService & GraphicsService();
}
