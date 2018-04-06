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
        Pixmap(std::shared_ptr<core::handle_t> handle);

        operator bool() const { return !!_handle; }
        std::shared_ptr<core::handle_t> handle() const { return _handle; }

    private:
        std::shared_ptr<core::handle_t> _handle = nullptr;
        si32_t _size;
    };
}
