#pragma once

#include "rendering/Text.h"

namespace ui
{
	class IWindow;

	class Scene
	{
	public:
		Scene();
		~Scene();

		std::shared_ptr<ui::IWindow> createWindow();
		drawing::GraphicsService & graphicsService();
		
		std::shared_ptr<drawing::Text> createTextComponent();
		std::shared_ptr<drawing::Image> createImage();
		std::shared_ptr<drawing::Path> createPath();
		
		drawing::fontmetrics font_metrics(const drawing::font & font);
		drawing::fontmetrics default_fontmetrics();
		
	private:
		std::vector<std::shared_ptr<rendering::Component>> _components;
	};
}
