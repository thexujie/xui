#pragma once
#include "ui/Control.h"

namespace ui::controls
{
    class Image : public Control
    {
    public:
        Image();
        explicit Image(std::string path);
        ~Image();

        void setImage(std::string path);
        void setImage(std::shared_ptr<drawing::Image> image);
        void setImageSize(const core::vec2<core::dimensionf> & size);
        void setImageFitting(const core::vec2<image_fitting> & fitting) { _image_fitting = fitting; }
        const core::vec2<image_fitting> & imageFitting() const { return _image_fitting; }
        core::sizef contentSize() const override;
        void draw(drawing::Graphics & graphics, const core::rectf & clip) const override;

    public:
        void onRectChanged(const core::rectf & from, const core::rectf & to) override;

    private:
        core::sizef _imageSize() const;

    private:
        std::shared_ptr<drawing::Image> _image;
        core::attribute<core::vec2<core::dimensionf>> _image_size;
        core::attribute<core::vec4f> _image_clip;
        core::vec2<image_fitting> _image_fitting = core::vec2<image_fitting>(image_fitting::none, image_fitting::none);
    };
}
