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

    core::si32f Image::contentSize() const
    {
        if (!_image_size)
            return _image ? _image->size().to<float32_t>() : core::si32f();

        return map(_image_size);
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
