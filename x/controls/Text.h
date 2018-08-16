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

        core::si32f contentSize() const override;
        void updateContent(std::shared_ptr<component::View> & view) override;

    private:
        // 确保 textBlob 已经被创建
        void _confirmBlob() const;

    private:
        std::string _text;
        std::shared_ptr<graphics::TextBlob> _textBlob;

        std::shared_ptr<renderables::Text> _text_obj;
    };
}
