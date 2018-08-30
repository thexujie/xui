#pragma once
#include "ui/Control.h"

namespace ui::controls
{
    class TextBox : public Control
    {
    public:
        TextBox();
        TextBox(std::string text);
        ~TextBox();

        static void propertyTableCallback(core::property_table & properties);
        void propertyTable(core::property_table & properties) override;

        void setText(const std::string & text);
        const std::string & text() const { return _text; }

        core::si32f contentSize() const override;
        void updateContent() override;
        std::string styleName() const override;
    public:
        void onMouseEnter(const component::mosue_state & state);
        void onMouseMove(const component::mosue_state & state);
        void onMouseLeave(const component::mosue_state & state);
        void onMouseDown(const component::mosue_state & state);
        void onMouseUp(const component::mosue_state & state);
        void onFocus();
        void onBlur();

    private:
        // 确保 textBlob 已经被创建
        void _confirmBlob() const;

    private:
        std::string _text;
        std::shared_ptr<graphics::TextBlob> _textblob;

        std::shared_ptr<renderables::Text> _text_obj;
        std::shared_ptr<renderables::Line> _cursor_obj;
        std::shared_ptr<core::property_animation> _cursor_anim;
        std::shared_ptr<component::Interactable> _input_obj;
    };
}
