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
        virtual core::error_e Clear(core::color32 color) = 0;
        virtual core::error_e DrawLine(pt32_t start, pt32_t end, color32 color, float32_t width) = 0;
        virtual core::error_e DrawRect(rc32_t rect, color32 color, float32_t width) = 0;
        virtual core::error_e FillRect(rc32_t rect, color32 color) = 0;
        virtual core::error_e DrawString(std::string str, core::color32 color, font font, pt32_t point, int32_t flags) = 0;
        virtual core::error_e DrawString(std::string str, core::color32 color, font font, rc32_t rect, int32_t flags) = 0;

        virtual core::error_e DrawImage(graphics::IImage & image, pt32_t point, int32_t flags) = 0;
        virtual core::error_e DrawImage(graphics::IImage & image, rc32_t rect, int32_t flags) = 0;
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
