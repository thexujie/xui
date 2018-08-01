#include "stdafx.h"
#include "Image.h"

namespace controls
{
    Image::Image()
    {
        
    }

    Image::Image(std::string path) : _image(std::make_shared<graphics::Image>(path))
    {

    }

    Image::~Image()
    {

    }

    void Image::setImage(std::string path)
    {
        _image = std::make_shared<graphics::Image>(path);
    }

    void Image::setImage(std::shared_ptr<graphics::Image> image)
    {
        _image = image;
    }

    void Image::setImageSize(const core::vec2<core::dimensionf> & size)
    {
        _image_size = size;
    }

    core::si32f Image::contentSize() const
    {
        if (_image_size.aviliable())
            return calc(_image_size.value);
        return _image ? _image->size().to<float32_t>() : core::si32f();
    }

    core::si32f Image::_imageSize() const
    {
        if (!_image_size)
            return _image ? _image->size().to<float32_t>() : core::si32f();
        else
        {
            // ×ÔÊÊÓ¦¿í¶È
            attribute<core::vec2<core::dimensionf>> _size = _image_size;
            if (_image_size.value.cx.nan() && !_image_size.value.cy.nan())
                _size.value.cx = _image_size.value.cy * _image->aspect();
            else if (!_image_size.value.cx.nan() && _image_size.value.cy.nan())
                _size.value.cy = _image_size.value.cx / _image->aspect();
            else if (_image_size.value.cx.nan() && _image_size.value.cy.nan())
            {
                _size.value.cx = core::unit_dot(_image->width());
                _size.value.cy = core::unit_dot(_image->height());
            }
            else {}
            return calc(_size);
        }
    }

    void Image::updateContent(std::shared_ptr<View> & view)
    {
        if (_image)
        {
            auto item = std::make_shared<renderables::Image>(_image);
            item->setRect(contentBox());
            item->setImageSize(_imageSize());
            item->setImageFitting(_image_fitting);
            view->insert(0, item);
        }
    }

    void Image::enteringScene(std::shared_ptr<component::Scene> & scene)
    {
        Control::enteringScene(scene);
    }

    void Image::enterScene(std::shared_ptr<component::Scene> & scene)
    {
        Control::enterScene(scene);
    }

    void Image::leaveScene(std::shared_ptr<component::Scene> & scene)
    {
        Control::leaveScene(scene);
    }

    void Image::onRectChanged(const core::rc32f & from, const core::rc32f & to)
    {
        Control::onRectChanged(from, to);
    }
}
