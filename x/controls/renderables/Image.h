#pragma once
#include "controls/component/Renderable.h"
#include "controls/attribute.h"

namespace controls::renderables
{
    class Image : public component::Renderable
    {
    public:
        Image();
        Image(std::shared_ptr<graphics::Image> image);
        ~Image();

        void setImage(std::shared_ptr<graphics::Image> image);
        void setImageSize(const core::si32f & size) { _size = size; }
        void setRepeat(image_repeat repeat) { _repeat = repeat; }

        void render(graphics::Graphics & graphics) const;

    private:
        std::shared_ptr<graphics::Image> _image;
        image_repeat _repeat;
        core::si32f _size;
    };
}
