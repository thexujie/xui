#pragma once
#include "ui/Control.h"

namespace ui::controls
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
        // ȷ�� textBlob �Ѿ�������
        void _confirmBlob() const;

    private:
        std::string _text;
        std::shared_ptr<graphics::TextBlob> _textBlob;

        std::shared_ptr<renderables::Text> _text_obj;
    };
}
