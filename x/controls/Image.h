#pragma once
#include "renderables/Image.h"
#include "Control.h"

namespace controls
{
    class Image : public Control
    {
    public:
        Image();
        explicit Image(std::string path);
        ~Image();

        void setImage(std::string path);
        void setImage(std::shared_ptr<graphics::Image> image);

    public:
        void enteringScene(std::shared_ptr<component::Scene> & scene) override;
        void enterScene(std::shared_ptr<component::Scene> & scene) override;
        void leaveScene(std::shared_ptr<component::Scene> & scene) override;

    private:
        std::shared_ptr<renderables::Image> _image;
    };
}
