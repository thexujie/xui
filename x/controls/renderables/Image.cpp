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
        Renderable::render(graphics);
        if (!_image)
            return;

        if(!_image_size.empty())
            graphics.drawImage(*_image, { _rect.pos, _image_size });
        else
            graphics.drawImage(*_image, _rect);
    }
}
