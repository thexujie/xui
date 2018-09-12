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
        std::string styleName() const override;

        void render(drawing::Graphics & graphics, const drawing::Region & region) const override;
    public:
        void onMouseEnter(const input_state & state);
        void onMouseMove(const input_state & state);
        void onMouseLeave(const input_state & state);
        void onMouseDown(const input_state & state, ui::mouse_button button);
        void onMouseUp(const input_state & state, ui::mouse_button button);

    private:
        // ȷ�� textBlob �Ѿ�������
        void _confirmBlob() const;

    private:
        std::string _text;
        mutable std::shared_ptr<drawing::TextBlob> _textBlob;
    };
}
