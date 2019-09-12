#include "stdafx.h"
#include "View.h"

namespace ui::rendering
{
	View::View() {}
	SceneView::~View() {}
	
	void View::addComponent(std::shared_ptr<Component> component)
	{
		_components.push_back(component);
	}

}
