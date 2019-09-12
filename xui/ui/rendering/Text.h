#pragma once

#include "Component.h"

namespace ui::rendering
{
	class TextComponent : public Component
	{
	public:
		TextComponent();
		~TextComponent();

		virtual void setText(std::u8string_view text, const drawing::font & font, core::color color) {}
		virtual core::sizef bounds() const { return {}; }
	};
}
