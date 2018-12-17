#include "stdafx.h"
#include "Image.h"

namespace ui::controls
{
    Image::Image()
    {
        _interactable = false;
    }

    Image::Image(std::string path) : _image(std::make_shared<drawing::Image>(path))
    {
        _interactable = false;
    }

    Image::~Image()
    {

    }

    void Image::setImage(std::string path)
    {
        _image = std::make_shared<drawing::Image>(path);
        refresh();
    }

    void Image::setImage(std::shared_ptr<drawing::Image> image)
    {
        _image = image;
        refresh();
    }

    void Image::setImageSize(const core::vec2<core::dimenf> & size)
    {
        _image_size = size;
        refresh();
    }

    core::sizef Image::_imageSize() const
    {
        if (!_image_size)
            return _image ? _image->size().to<float32_t>() : core::sizef();
        else
        {
            // 自适应宽度
            core::attribute<core::vec2<core::dimenf>> _size = _image_size;
            if (_image_size.value.cx.nan() && !_image_size.value.cy.nan())
                _size.value.cx = _image_size.value.cy * _image->aspect();
            else if (!_image_size.value.cx.nan() && _image_size.value.cy.nan())
                _size.value.cy = _image_size.value.cx / _image->aspect();
            else if (_image_size.value.cx.nan() && _image_size.value.cy.nan())
            {
                _size.value.cx = core::unit_dot(float32_t(_image->width()));
                _size.value.cy = core::unit_dot(float32_t(_image->height()));
            }
            else {}
            return calc(_size);
        }
    }

    void Image::update()
    {
        if (_image_size.available())
            setContentSize(calc(_image_size.value));
        else
            setContentSize(_image ? _image->size().to<float32_t>() : core::sizef());
    }

    void Image::paint(drawing::Graphics & graphics, const core::rectf & clip) const
    {
        graphics.save();
        auto box = contentBox();
        graphics.setClipRect(box);

        auto image_size = _imageSize();
        core::rectf rect = box;
        if (_image_fitting.x == image_fitting::scale)
            rect.cx = float32_t(_image->width());
        else
            rect.cx = image_size.empty() ? float32_t(_image->width()) : image_size.cx;

        if (_image_fitting.y == image_fitting::scale)
            rect.cy = float32_t(_image->height());
        else
            rect.cy = image_size.empty() ? float32_t(_image->height()) : image_size.cy;

        while (true)
        {
            // 一行一行来
            while (true)
            {
                if (_image_clip.available())
                    graphics.drawImage(*_image, rect, _image_clip);
                else
                    graphics.drawImage(*_image, rect);

                if (_image_fitting.x != image_fitting::repeat)
                    break;

                rect.x += rect.cx;
                if (rect.x >= box.right())
                    break;
            }
            if (_image_fitting.y != image_fitting::repeat)
                break;

            rect.x = box.x;
            rect.y += rect.cy;
            if (rect.y >= box.bottom())
                break;
        }
        graphics.restore();
    }

    void Image::onRectChanged(const core::rectf & from, const core::rectf & to)
    {
        Control::onRectChanged(from, to);
    }
}
