#include "stdafx.h"
#include "Pixmap.h"

namespace graphics
{
    using namespace core;
    using namespace core::math;

    Pixmap::Pixmap(si32_t size)
    {
        _pixmap = GraphicsService().CreatePixmap(size);
    }

    Pixmap::Pixmap(std::shared_ptr<IGraphicsPixmap> pixmap) : _pixmap(pixmap)
    {
        
    }

    void Pixmap::Save(std::string path)
    {
        if(!_pixmap)
        {
            _state = error_state;
            return;
        }

        _state = _pixmap->Save(path);
    }
}

