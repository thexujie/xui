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
    }

    void Image::setImage(std::shared_ptr<graphics::Image> image)
    {
        _image->setImage(image);
    }

    void Image::enteringScene(std::shared_ptr<component::Scene> & scene)
    {
        _image->setPos(pos());
        _image->setSize(size());
        Control::enteringScene(scene);
    }

    void Image::enterScene(std::shared_ptr<component::Scene> & scene)
    {
        scene->addComponent(std::dynamic_pointer_cast<component::Component>(_image));
        Control::enterScene(scene);
    }

    void Image::leaveScene(std::shared_ptr<component::Scene> & scene)
    {
        scene->removeComponent(std::dynamic_pointer_cast<component::Component>(_image));
        Control::leaveScene(scene);
    }
}
