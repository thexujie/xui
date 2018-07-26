#pragma once
#include "Control.h"

namespace controls
{
    class Text : public Control
    {
    public:
        Text();
        Text(std::string text);
        ~Text();

        virtual void enterScene(std::shared_ptr<component::Scene> & scene);
        core::si32f contentSize() const;
        virtual void update();

    private:
        std::string _text;
        std::shared_ptr<graphics::TextBlob> _textBlob;
    };
}
