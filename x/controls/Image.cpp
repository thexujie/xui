#include "stdafx.h"
#include "Image.h"

namespace controls
{
    Image::Image():_image(std::make_shared<renderables::Image>())
    {
        
    }

    Image::Image(std::string path) : _image(std::make_shared<renderables::Image>(std::make_shared<graphics::Image>(path)))
    {

    }

    Image::~Image()
    {

    }

    void Image::setImage(std::string path)
    {
        auto image = std::make_shared<graphics::Image>(path);
        _image->setImage(image);
        _image->setPos(_rect.center());
        _image->setSize(_rect.size);
    }

    void Image::setImage(std::shared_ptr<graphics::Image> image)
    {
        _image->setImage(image);
        _image->setPos(_rect.center());
        _image->setSize(_rect.size);
    }

    void Image::enteringScene(std::shared_ptr<component::Scene> & scene)
    {
        _image->setPos(_rect.center());
        _image->setSize(_rect.size);
        Control::enteringScene(scene);
    }

    void Image::enterScene(std::shared_ptr<component::Scene> & scene)
    {
        scene->addRenderable(_image);
        Control::enterScene(scene);
    }

    void Image::leaveScene(std::shared_ptr<component::Scene> & scene)
    {
        scene->removeRenderable(_image);
        Control::leaveScene(scene);
    }

    void Image::onRectChanged(const core::rc32f & from, const core::rc32f & to)
    {
        _image->setPos(_rect.center());
        _image->setSize(_rect.size);
        Control::onRectChanged(from, to);
    }
}
