#pragma once
#include "renderables/Image.h"
#include "View.h"

namespace controls
{
    class Image : public View
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
        void onRectChanged(const core::rc32f & from, const core::rc32f & to) override;

    private:
        std::shared_ptr<renderables::Image> _image;
    };
}
