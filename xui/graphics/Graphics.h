#pragma once

#include "GraphicsService.h"
#include "Pixmap.h"

namespace graphics
{
    class Graphics
    {
    public:
        Graphics() = default;
        ~Graphics() = default;

        Graphics(si32_t size);
        Graphics(std::shared_ptr<IGraphics> graphics);
        Graphics(std::shared_ptr<Pixmap> pixmap);

    public:
        void Clear();

    private:
        std::shared_ptr<IGraphics> _graphics;
    };
}
