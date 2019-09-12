#pragma once

#include "rendering/Component.h"

namespace ui
{
	class TextComponent;

	class SceneView
	{
	public:
		SceneView();
		~SceneView();

		void addComponent(std::shared_ptr<rendering::Component> component);

	private:
		std::vector<std::shared_ptr<rendering::Component>> _components;
	};
}
