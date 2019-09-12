#include "stdafx.h"
#include "Scene.h"

namespace ui
{
	Scene::Scene() {}
	Scene::~Scene() {}

	std::shared_ptr<ui::IWindow> Scene::createWindow()
	{
		return nullptr;
	}
	
	drawing::GraphicsService & Scene::graphicsService()
	{
		return *(drawing::GraphicsService *)0;
	}
	
	std::shared_ptr<drawing::Text> Scene::createTextComponent()
	{
		return nullptr;
	}

	std::shared_ptr<drawing::Image> Scene::createImage()
	{
		return nullptr;
	}

	std::shared_ptr<drawing::Path> Scene::createPath()
	{
		return nullptr;
	}
	
	drawing::fontmetrics Scene::font_metrics(const drawing::font & font)
	{
		return {};
	}
	
	drawing::fontmetrics Scene::default_fontmetrics()
	{
		return {};
	}
}
