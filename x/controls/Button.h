#pragma once
#include "Control.h"

namespace controls
{
    class Button : public Control
    {
    public:
        Button();
        ~Button();

        core::si32f contentSize() const;
        virtual void layoutContent();
        virtual void update();

    private:
        std::string _text;
        std::shared_ptr<graphics::TextBlob> _textBlob;
    };
}
