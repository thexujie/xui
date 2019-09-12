#include "stdafx.h"
#include "path_source.h"

namespace drawing
{
    using namespace core;
	
    path_source::~path_source()
    {
    }

    void path_source::clear()
    {
		_buffer.clear();
    }

    void path_source::close()
    {
		_allocate<command>() = command::close;
    }

    void path_source::move(const core::pointf & pos)
    {
		_allocate<std::tuple<command, core::pointf>>() = { command::close, pos };
    }

    void path_source::line(const core::pointf & pos)
    {
		_allocate<std::tuple<command, core::pointf>>() = { command::line, pos };
    }

    void path_source::arc(const core::pointf & pos0, const core::pointf & pos1, float32_t radius)
    {
		_allocate<std::tuple<command, core::pointf, core::pointf, float32_t>>() = { command::line, pos0, pos1, radius };
    }

    void path_source::transform(const core::float3x2 & matrix)
    {
    }

    void path_source::transform(const core::float3x2 & matrix, path_source & output)
    {
    }
}
