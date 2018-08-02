#pragma once
#include "Control.h"
#include "interactables/MouseRectangle.h"

namespace controls
{
    class Button : public Control
    {
    public:
        Button();
        Button(std::string text);
        ~Button();

        core::si32f contentSize() const;
        void updateContent(std::shared_ptr<component::View> & view) override;

    private:
        // 确保 textBlob 已经被创建
        void _confirmBlob() const;

    private:
        std::string _text;
        std::shared_ptr<graphics::TextBlob> _textBlob;

        std::shared_ptr<renderables::Text> _text_obj;
        std::shared_ptr<interactables::MouseRectangle> _mosuerectangle;
    };
}
