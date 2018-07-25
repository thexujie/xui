#include "stdafx.h"
#include "Image.h"

namespace controls::renderables
{
    Image::Image()
    {

    }

    Image::Image(std::shared_ptr<graphics::Image> image) :_image(image)
    {
        
    }

    Image::~Image()
    {

    }

    void Image::setImage(std::shared_ptr<graphics::Image> image)
    {
        _image = image;
    }

    void Image::render(graphics::Graphics & graphics) const
    {
        if (!_image)
            return;

        if(!_size.empty())
            graphics.drawImage(*_image, { pos() - _size * 0.5f, _size });
    }
}
