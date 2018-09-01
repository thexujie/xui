#pragma once
#include "ui/Control.h"

namespace ui::controls
{
    class Button : public Control
    {
    public:
        Button();
        Button(std::string text);
        ~Button();

        static void propertyTableCallback(core::property_table & properties);
        void propertyTable(core::property_table & properties) override;

        void setText(const std::string & text);
        const std::string & text() const { return _text; }

        core::si32f contentSize() const override;
        void updateContent() override;
        std::string styleName() const override;
    public:
        void onMouseEnter(const mosue_state & state);
        void onMouseMove(const mosue_state & state);
        void onMouseLeave(const mosue_state & state);
        void onMouseDown(const mosue_state & state);
        void onMouseUp(const mosue_state & state);

    private:
        // 确保 textBlob 已经被创建
        void _confirmBlob() const;

    private:
        std::string _text;
        mutable std::shared_ptr<graphics::TextBlob> _textBlob;

        std::shared_ptr<renderables::Text> _text_obj;
    };
}
