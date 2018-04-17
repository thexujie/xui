#pragma once

#include "core/math/vec2.h"
#include "core/math/rect.h"
#include "image/image.h"
#include "text/text.h"
#include "raster/path.h"

namespace graphics
{
    using core::color32;
    using core::math::si32_t;
    using core::math::pt32_t;
    using core::math::rc32_t;

    struct pixmap_buffer
    {
        byte_t * data = nullptr;
        si32_t size;
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
        virtual si32_t size() const = 0;
        virtual core::error_e Save(std::string path) const = 0;
    };

    class IGraphicsImage : public IGraphicsItem
    {
    public:
        virtual ~IGraphicsImage() = default;
        virtual si32_t size() const = 0;
        virtual image::format cmode() const = 0;

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

        virtual void PushOrign(core::math::pt32_t point) = 0;
        virtual core::math::pt32_t GetOrign() const = 0;
        virtual void PopOrign() = 0;
        virtual void PushClip(core::math::rc32_t rect) = 0;
        virtual core::math::rc32_t GetClip() const = 0;
        virtual void PopClip() = 0;

        virtual void Clear(core::color32 color) = 0;

        virtual void DrawString(std::string str, core::math::pt32_t point, graphics::text::font font, core::color32 color) = 0;

        virtual void DrawImage(graphics::IGraphicsImage & image, pt32_t point) = 0;
        virtual void DrawImage(graphics::IGraphicsImage & image, core::math::pt32_t point, core::math::rc32_t region) = 0;
        virtual void DrawImage(graphics::IGraphicsImage & image, rc32_t rect) = 0;
        virtual void DrawImage(graphics::IGraphicsImage & image, core::math::rc32_t rect, core::math::rc32_t region) = 0;
        virtual void DrawString(graphics::IGraphicsString & str, core::math::pt32_t point) = 0;

        virtual void FillPath(graphics::raster::path & path, core::color32 color) = 0;

        virtual graphics::text::fontmetrics GetFontMetrics(graphics::text::font font) = 0;
        virtual core::math::si32_t MeasureString(std::string str, graphics::text::font font) = 0;
    };

    class IGraphicsService : public core::Object
    {
    public:
        virtual ~IGraphicsService() = default;
        virtual std::shared_ptr<graphics::IGraphicsPixmap> CreatePixmap(core::math::si32_t) = 0;
        virtual std::shared_ptr<graphics::IGraphicsImage> CreateImage(std::string path) = 0;
        virtual std::shared_ptr<graphics::IGraphicsImage> CreateString(std::string str, graphics::text::font font, core::color32 color) = 0;
        virtual std::shared_ptr<graphics::IGraphics> CreateGraphics(std::shared_ptr<core::Object> pixmap) = 0;
    };

    IGraphicsService & GraphicsService();
}
