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

    public:
        void onMouseEnter(const component::mosue_state & state);
        void onMouseMove(const component::mosue_state & state);
        void onMouseLeave(const component::mosue_state & state);
        void onMouseDown(const component::mosue_state & state);
        void onMouseUp(const component::mosue_state & state);

    private:
        // ȷ�� textBlob �Ѿ�������
        void _confirmBlob() const;

    private:
        std::string _text;
        std::shared_ptr<graphics::TextBlob> _textBlob;

        std::shared_ptr<renderables::Text> _text_obj;
        std::shared_ptr<interactables::MouseRectangle> _mosuerectangle;
    };
}
