#include "stdafx.h"
#include "Image.h"

namespace ui::renderables
{
    void Image::setImage(std::shared_ptr<graphics::Image> image)
    {
        if(_image != image)
        {
            _image = image;
            invalid();
        }
    }

    void Image::render(graphics::Graphics & graphics) const
    {
        Renderable::render(graphics);
        if (!_image)
            return;

        graphics.save();
        graphics.setClipRect(_rect);
        core::rc32f rect(_rect.pos, _image_size);

        if (_fitting.x == image_fitting::scale)
            rect.cx = float32_t(_image->width());
        else
            rect.cx = _image_size.empty() ? float32_t(_image->width()) : _image_size.cx;

        if (_fitting.y == image_fitting::scale)
            rect.cy = float32_t(_image->height());
        else
            rect.cy = _image_size.empty() ? float32_t(_image->height()) : _image_size.cy;

        while(true)
        {
            // 一行一行来
            while (true)
            {
                if(_image_clip.empty())
                    graphics.drawImage(*_image, rect);
                else
                    graphics.drawImage(*_image, rect, _image_clip);

                if (_fitting.x != image_fitting::repeat)
                    break;

                rect.x += rect.cx;
                if (rect.x >= _rect.right())
                    break;
            }
            if (_fitting.y != image_fitting::repeat)
                break;

            rect.x = _rect.x;
            rect.y += rect.cy;
            if (rect.y >= _rect.bottom())
                break;
        }
        graphics.restore();
    }
}
