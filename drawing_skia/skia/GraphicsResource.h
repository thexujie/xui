#pragma once

class SkPath;

namespace drawing::skia
{
	class GraphicsObject
	{
	public:
		GraphicsObject() = default;
		virtual ~GraphicsObject() = default;
	};
	
    class GraphicsResource
    {
    public:
		GraphicsResource() = default;
		virtual ~GraphicsResource() = default;
    };
}
