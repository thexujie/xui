#pragma once
#include "ui/component/Renderable.h"

namespace ui::renderables
{
    enum class image_fitting
    {
        none = 0,
        scale,
        repeat,
    };

    class Image : public component::Renderable
    {
    public:
        Image(std::shared_ptr<ui::Control> control) : component::Renderable(control) {}
        Image(std::shared_ptr<ui::Control> control, std::shared_ptr<graphics::Image> image) : component::Renderable(control), _image(image) {}
        ~Image() {}

        void setImage(std::shared_ptr<graphics::Image> image);
        void setImageSize(const core::si32f & size) { _image_size = size; }
        void setImageFitting(const core::vec2<image_fitting> & fitting) { _fitting = fitting; }

        void render(graphics::Graphics & graphics) const override;

    private:
        std::shared_ptr<graphics::Image> _image;
        core::vec2<image_fitting> _fitting;
        core::rc32f _image_clip;
        core::si32f _image_size;
    };
}
