#include "stdafx.h"
#include "SceneView.h"

namespace ui
{
	SceneView::SceneView() {}
	SceneView::~SceneView() {}
	
	void SceneView::addComponent(std::shared_ptr<rendering::Component> component)
	{
		_components.push_back(component);
	}
}
