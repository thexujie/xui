#include "stdafx.h"
#include "Pixmap.h"

namespace graphics
{
    Pixmap::Pixmap(core::si32i size)
    {
        _pixmap = GraphicsService().CreatePixmap(size);
    }

    Pixmap::Pixmap(std::shared_ptr<IGraphicsPixmap> pixmap) : _pixmap(pixmap) { }

    pixmap_buffer Pixmap::buffer() const
    {
        if (!_pixmap)
            return {};
        return _pixmap->buffer();
    }

    core::si32i Pixmap::size() const
    {
        if (!_pixmap)
            return {};
        return _pixmap->size();
    }

    void Pixmap::Save(std::string path)
    {
        if (!_pixmap)
        {
            _state = core::error_state;
            return;
        }

        _state = _pixmap->Save(path);
    }
}
