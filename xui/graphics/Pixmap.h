#pragma once

#include "GraphicsService.h"

namespace graphics
{
    class Pixmap
    {
    public:
        Pixmap() = default;
        ~Pixmap() = default;

        Pixmap(si32_t size);
        Pixmap(std::shared_ptr<IPixmap> pixmap);

        operator bool() const { return !!_pixmap; }
        std::shared_ptr<IPixmap> handle() const { return _pixmap; }

        void Save(std::string path);
    private:
        core::error_e _state = core::error_ok;
        std::shared_ptr<IPixmap> _pixmap = nullptr;
        si32_t _size;
    };
}
