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

        core::si32f contentSize() const;
        virtual void layoutContent();
        virtual void update();

    public:
        void enteringScene(std::shared_ptr<component::Scene> & scene) override;
        void enterScene(std::shared_ptr<component::Scene> & scene) override;
        void leaveScene(std::shared_ptr<component::Scene> & scene) override;
        void onRectChanged(const core::rc32f & from, const core::rc32f & to) override;

    private:
        std::shared_ptr<graphics::Image> _image;
        attribute<core::vec2<unit_value<float32_t>>> _image_size;
        image_repeat _image_repeat = image_repeat::repeat;
    };
}
