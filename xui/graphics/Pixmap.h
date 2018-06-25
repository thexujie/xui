#pragma once

#include "GraphicsService.h"

namespace graphics
{
    class Pixmap
    {
    public:
        Pixmap() = default;
        ~Pixmap() = default;

        Pixmap(core::math::si32i size);
        Pixmap(std::shared_ptr<IGraphicsPixmap> pixmap);

        operator bool() const { return !!_pixmap; }
        std::shared_ptr<IGraphicsPixmap> handle() const { return _pixmap; }
        pixmap_buffer buffer() const;
        core::math::si32i size() const;
        void Save(std::string path);
    private:
        core::error_e _state = core::error_ok;
        std::shared_ptr<IGraphicsPixmap> _pixmap = nullptr;
        core::math::si32i _size;
    };
}
