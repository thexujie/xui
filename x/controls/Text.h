#pragma once
#include "Control.h"

namespace controls
{
    class Text : public Control
    {
    public:
        Text();
        ~Text();

        core::si32f contentSize() const;
        virtual void layoutContent();
        virtual void update();

    private:
        std::string _text;
        std::shared_ptr<graphics::TextBlob> _textBlob;
    };
}
