#pragma once
#include "renderables/Image.h"
#include "Control.h"

namespace ui
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
        void setImageFitting(const core::vec2<renderables::image_fitting> & fitting) { _image_fitting = fitting; }
        const core::vec2<renderables::image_fitting> & imageFitting() const { return _image_fitting; }
        core::si32f contentSize() const override;

        core::si32f _imageSize() const;
        void updateContent(std::shared_ptr<component::View> & view) override;

    public:
        void enteringScene(std::shared_ptr<component::Scene> & scene) override;
        void enterScene(std::shared_ptr<component::Scene> & scene) override;
        void leaveScene(std::shared_ptr<component::Scene> & scene) override;
        void onRectChanged(const core::rc32f & from, const core::rc32f & to) override;

    private:
        std::shared_ptr<graphics::Image> _image;
        core::attribute<core::vec2<core::dimensionf>> _image_size;
        core::vec2<renderables::image_fitting> _image_fitting = core::vec2<renderables::image_fitting>(renderables::image_fitting::none, renderables::image_fitting::none);

        std::shared_ptr<renderables::Image> _image_obj;
    };
}
