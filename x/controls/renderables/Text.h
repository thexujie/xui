#pragma once
#include "controls/component/Renderable.h"
#include "controls/attribute.h"

namespace controls::renderables
{
    class Text : public component::Renderable
    {
    public:
        Text();
        Text(std::shared_ptr<graphics::TextBlob> text);
        ~Text();

        void setText(std::shared_ptr<graphics::TextBlob> text);
        void render(graphics::Graphics & graphics) const;

    private:
        std::shared_ptr<graphics::TextBlob> _text;
    };
}