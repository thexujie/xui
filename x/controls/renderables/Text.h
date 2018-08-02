#pragma once
#include "controls/component/Renderable.h"

namespace controls::renderables
{
    class Text : public component::Renderable
    {
    public:
        Text(std::shared_ptr<component::View> view);
        Text(std::shared_ptr<component::View> view, std::shared_ptr<graphics::TextBlob> text);
        ~Text();

        void setText(std::shared_ptr<graphics::TextBlob> text);
        void render(graphics::Graphics & graphics) const;

    private:
        std::shared_ptr<graphics::TextBlob> _text;
    };
}
