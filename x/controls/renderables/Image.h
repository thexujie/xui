#pragma once
#include "controls/component/Renderable.h"

namespace controls::renderables
{
    class Image : public component::Renderable
    {
    public:
        Image();
        Image(std::shared_ptr<graphics::Image> image);
        ~Image();

        void setSize(const core::si32f & si);
        core::si32f size() const { return _size; }

        void setImage(std::shared_ptr<graphics::Image> image);

        void render(graphics::Graphics & graphics) const;

    private:
        std::shared_ptr<graphics::Image> _image;
        core::si32f _size;
    };
}
