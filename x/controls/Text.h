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

        core::si32f expectContentSize() const;
        void enteringScene(std::shared_ptr<component::Scene> & scene) override;
        void enterScene(std::shared_ptr<component::Scene> & scene) override;
        void update() override;

    private:
        // 确保 textBlob 已经被创建
        void _confirmBlob() const;

    private:
        std::string _text;
        std::shared_ptr<graphics::TextBlob> _textBlob;
    };
}
