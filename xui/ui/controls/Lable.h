#pragma once
#include "ui/Control.h"

namespace ui::controls
{
    class Lable : public Control
    {
    public:
        Lable();
        ~Lable();

		void onEnterScene() override;
		void onLeaveScene() override;
        void update() override;
        void paint(drawing::Graphics & graphics, const core::rectf & clip) const override;

    public:
        void setText(const std::u8string & text);
        const std::u8string & text() const { return _text; }

    private:
		std::u8string _text;
		std::shared_ptr<drawing::Text> _text_object;
    };
}
