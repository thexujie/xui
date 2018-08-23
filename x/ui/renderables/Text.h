#pragma once
#include "ui/component/Renderable.h"

namespace ui::renderables
{
    class Text : public component::Renderable
    {
    public:
        Text(std::shared_ptr<ui::Control> control) : component::Renderable(control) {}
        Text(std::shared_ptr<ui::Control> control, std::shared_ptr<graphics::TextBlob> text) : component::Renderable(control), _text(text) {}
        ~Text();

        void setTextBlob(std::shared_ptr<graphics::TextBlob> text);
        void render(graphics::Graphics & graphics) const override;

    private:
        std::shared_ptr<graphics::TextBlob> _text;
    };
}
