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
        void setImageSize(const core::vec2<core::dimensionf> & size);
        void setImageFitting(const core::vec2<image_fitting> & fitting) { _image_fitting = fitting; }
        const core::vec2<image_fitting> & imageFitting() const { return _image_fitting; }
        core::si32f contentSize() const;

        core::si32f _imageSize() const;
        virtual void updateContent(std::shared_ptr<View> & view);

    public:
        void enteringScene(std::shared_ptr<component::Scene> & scene) override;
        void enterScene(std::shared_ptr<component::Scene> & scene) override;
        void leaveScene(std::shared_ptr<component::Scene> & scene) override;
        void onRectChanged(const core::rc32f & from, const core::rc32f & to) override;

    private:
        std::shared_ptr<graphics::Image> _image;
        attribute<core::vec2<core::dimensionf>> _image_size;
        core::vec2<image_fitting> _image_fitting = core::vec2<image_fitting>(image_fitting::none, image_fitting::none);
    };
}
