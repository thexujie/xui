#pragma once

#include "core/math/vec2.h"

namespace graphics
{
    using core::math::si32_t;

    class IGraphics
    {
    public:
        virtual ~IGraphics() = default;
        virtual void Clear() = 0;
    };

    class IGraphicsService
    {
    public:
        virtual ~IGraphicsService() = default;
        virtual std::shared_ptr<core::handle_t> CreatePixmap(core::math::si32_t) = 0;
        virtual std::shared_ptr<graphics::IGraphics> CreateGraphics(std::shared_ptr<core::handle_t> pixmap) = 0;
    };

    IGraphicsService & GraphicsService();
}
