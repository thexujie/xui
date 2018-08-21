#pragma once
#include "ui/component/Renderable.h"

namespace ui::renderables
{
    class Text : public component::Renderable
    {
    public:
        Text();
        Text(std::shared_ptr<graphics::TextBlob> text);
        ~Text();

        void setTextBlob(std::shared_ptr<graphics::TextBlob> text);
        void render(graphics::Graphics & graphics) const override;

    private:
        std::shared_ptr<graphics::TextBlob> _text;
    };
}