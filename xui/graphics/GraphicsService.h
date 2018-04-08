#pragma once

#include "core/math/vec2.h"
#include "core/math/rect.h"
#include "font.h"
#include "image/image.h"

namespace graphics
{
    class Image;
    using core::color32;
    using core::math::si32_t;
    using core::math::pt32_t;
    using core::math::rc32_t;

    class IPixmap : public core::Object
    {
    public:
        virtual ~IPixmap() = default;
        virtual core::error_e Save(std::string path) const = 0;
    };

    class IImage : public core::Object
    {
    public:
        virtual ~IImage() = default;
        virtual si32_t size() const = 0;
        virtual image::cmode_e cmode() const = 0;

        virtual core::error_e Save(std::string path) const = 0;
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
        virtual void DrawLine(pt32_t start, pt32_t end, color32 color, float32_t width) = 0;
        virtual void DrawRect(rc32_t rect, color32 color, float32_t width) = 0;
        virtual void FillRect(rc32_t rect, color32 color) = 0;

        virtual void DrawString(std::string str, core::math::pt32_t point, graphics::font font, core::color32 color) = 0;
        virtual void DrawImage(graphics::IImage & image, pt32_t point) = 0;
        virtual void DrawImage(graphics::IImage & image, core::math::pt32_t point, core::math::rc32_t region) = 0;
        virtual void DrawImage(graphics::IImage & image, rc32_t rect) = 0;
        virtual void DrawImage(graphics::IImage & image, core::math::rc32_t rect, core::math::rc32_t region) = 0;

        virtual graphics::fontmetrics GetFontMetrics(graphics::font font) = 0;
        virtual core::math::si32_t MeasureString(std::string str, graphics::font font) = 0;
    };

    class IGraphicsService : public core::Object
    {
    public:
        virtual ~IGraphicsService() = default;
        virtual std::shared_ptr<graphics::IPixmap> CreatePixmap(core::math::si32_t) = 0;
        virtual std::shared_ptr<graphics::IImage> CreateImage(std::string path) = 0;
        virtual std::shared_ptr<graphics::IGraphics> CreateGraphics(std::shared_ptr<core::Object> pixmap) = 0;
    };

    IGraphicsService & GraphicsService();
}
