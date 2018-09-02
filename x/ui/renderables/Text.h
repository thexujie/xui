#pragma once
#include "ui/component/Renderable.h"

namespace ui::renderables
{
    class Text : public component::Renderable
    {
    public:
        Text(std::shared_ptr<ui::Control> control) : component::Renderable(control) {}
        Text(std::shared_ptr<ui::Control> control, std::shared_ptr<drawing::TextBlob> text) : component::Renderable(control), _text(text) {}
        ~Text();

        void setTextBlob(std::shared_ptr<drawing::TextBlob> text);
        void render(drawing::Graphics & graphics) const override;

    private:
        std::shared_ptr<drawing::TextBlob> _text;
    };
}
