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

    void Image::setImageSize(const core::vec2<core::unit_value<float32_t>> & size)
    {
        _image_size = size;
    }

    void Image::layoutContent()
    {
        
    }

    void Image::update()
    {
        Control::update();
        if (_image)
        {
            auto v = view();
            auto item = std::make_shared<renderables::Image>(_image);
            item->setRect(contentBox());
            item->setImageSize(contentSize());
            item->setRepeat(_image_repeat);
            v->insert(0, item);
        }
    }

    void Image::enteringScene(std::shared_ptr<component::Scene> & scene)
    {
        Control::enteringScene(scene);

        if (!_image_size)
            _view_content_size = _image ? _image->size().to<float32_t>() : core::si32f();
        else
        {
            // ×ÔÊÊÓ¦¿í¶È
            attribute<core::vec2<core::unit_value<float32_t>>> _size = _image_size;
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
            _view_content_size = map(_size);
        }
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
