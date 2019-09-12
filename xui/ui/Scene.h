#pragma once

#include "RadioGroup.h"

namespace ui
{
	class RadioGroup;
	class IWindow;

	class Scene
	{
	public:
		Scene();
		~Scene();

		std::shared_ptr<ui::IWindow> createWindow();
		drawing::GraphicsService & graphicsService();
		
		std::shared_ptr<drawing::Text> createText();
		std::shared_ptr<drawing::Image> createImage();
		std::shared_ptr<drawing::Path> createPath();
		
		drawing::fontmetrics font_metrics(const drawing::font & font);
		drawing::fontmetrics default_fontmetrics();


		std::shared_ptr<RadioGroup> radioGroup(const std::u8string & name);
		
	private:
		std::map<std::u8string, std::weak_ptr<RadioGroup>> _radio_groups;
	};
}
