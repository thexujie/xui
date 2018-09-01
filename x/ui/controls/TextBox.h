#pragma once
#include "ui/Control.h"

namespace ui::controls
{
    class TextBox : public Control
    {
    public:
        TextBox();
        ~TextBox();

        static void propertyTableCallback(core::property_table & properties);
        void propertyTable(core::property_table & properties) override;

        void setText(const std::string & text);
        const std::string & text() const { return _text; }

        core::si32f contentSize() const override;
        void updateContent() override;
        std::string styleName() const override;

    public:
        void onMouseEnter(const mosue_state & state) override;
        void onMouseMove(const mosue_state & state) override;
        void onMouseLeave(const mosue_state & state) override;
        void onMouseDown(const mosue_state & state) override;
        void onMouseUp(const mosue_state & state) override;
        void onFocus(std::shared_ptr<ImeContext> imecontext) override;
        void onBlur() override;
        void onChar(char32_t ch) override;

    private:
        void _updateIme();
        // ȷ�� textBlob �Ѿ�������
        void _confirmBlob() const;

    private:
        std::string _text;
        mutable std::shared_ptr<graphics::TextBlob> _textblob;

        std::shared_ptr<renderables::Text> _text_obj;
        std::shared_ptr<renderables::Line> _cursor_obj;
        std::shared_ptr<core::property_animation> _cursor_anim;

        std::shared_ptr<ImeContext> _imecontext;
    };
}
