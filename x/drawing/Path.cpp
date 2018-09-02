#include "stdafx.h"
#include "Path.h"
#include "core/io/filestream.h"

#include "skia/skia.h"
#include <SkPath.h>

namespace drawing
{
    using namespace core;

    Path::~Path()
    {
    }

    void Path::close()
    {
        if (_native)
            _native->close();
    }

    void Path::moveTo(const core::pt32f & pos)
    {
        _confirmNative();
        _native->moveTo(skia::from(pos));
    }

    void Path::lineTo(const core::pt32f & pos)
    {
        _confirmNative();
        _native->lineTo(skia::from(pos));
    }

    void Path::arcTo(const core::pt32f & pos0, const core::pt32f & po1, float32_t radius)
    {
        _confirmNative();
        _native->arcTo(skia::from(pos0), skia::from(po1), radius);
    }

    void Path::_confirmNative()
    {
        if (!_native)
            _native = std::make_shared<SkPath>();
    }

    core::rc32f Path::computeTightBounds() const
    {
        return skia::to(_native->computeTightBounds());
    }
}
