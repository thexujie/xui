#pragma once
#include "controls/component/Renderable.h"

namespace controls::renderables
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
        Image() {}
        Image(std::shared_ptr<graphics::Image> image) : _image(image) {}
        ~Image() {}

        void setImage(std::shared_ptr<graphics::Image> image);
        void setImageSize(const core::si32f & size) { _image_size = size; }
        void setImageFitting(const core::vec2<image_fitting> & fitting) { _fitting = fitting; }

        void render(graphics::Graphics & graphics) const;

    private:
        std::shared_ptr<graphics::Image> _image;
        core::vec2<image_fitting> _fitting;
        core::rc32f _image_clip;
        core::si32f _image_size;
    };
}
