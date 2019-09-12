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
	
	std::shared_ptr<drawing::Text> Scene::createText()
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

	std::shared_ptr<RadioGroup> Scene::radioGroup(const std::u8string & name)
	{
		auto iter = _radio_groups.find(name);
		if (iter == _radio_groups.end())
		{
			auto group = std::make_shared<RadioGroup>(name);
			_radio_groups[name] = group;
			return group;
		}
		else
		{
			auto ptr = iter->second.lock();
			if (!ptr)
			{
				auto group = std::make_shared<RadioGroup>(name);
				_radio_groups[name] = group;
				return group;
			}
			else
				return ptr;
		}
	}
}
