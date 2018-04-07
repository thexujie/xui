#include "stdafx.h"
#include "Image.h"


namespace graphics
{
    Image::Image(std::string path)
    {
        _image = GraphicsService().CreateImage(path);
    }


    si32_t Image::size() const
    {
        if (!_image)
            return {};
        return _image->size();
    }

    void Image::Save(std::string path) const
    {
        if (_image)
            _image->Save(path);
    }
}

