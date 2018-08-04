#include "stdafx.h"
#include "Region.h"
#include "core/io/filestream.h"

#include "skia/skia.h"
#include <SkRegion.h>

namespace graphics
{
    using namespace core;

    Region::~Region() { }

    void Region::clear()
    {
        if (_native)
            _native->setEmpty();
    }

    bool Region::empty() const
    {
        if (_native)
            return true;

        return _native->isEmpty();
    }

    void Region::setRect(const core::rc32i & rect)
    {
        _confirmNative();
        _native->setRect(skia::from(rect));
    }

    void Region::addRect(const core::rc32i & rect)
    {
        _confirmNative();
        _native->op(skia::from(rect), SkRegion::Op::kUnion_Op);
    }

    bool Region::intersects(const core::rc32i & rect) const
    {
        if (!_native)
            return false;

        return _native->intersects(skia::from(rect));
    }

    bool Region::intersects(const Region & region) const
    {
        if (!_native)
            return false;

        return _native->intersects(region.native());
    }

    bool Region::contains(const core::pt32i & point) const
    {
        if (!_native)
            return false;

        return _native->contains(point.x, point.y);
    }

    bool Region::contains(const core::rc32i & rect) const
    {
        if (!_native)
            return false;

        return _native->contains(skia::from(rect));
    }

    bool Region::contains(const Region & region) const
    {
        if (!_native)
            return false;

        return _native->contains(region.native());
    }

    core::rc32i Region::bounds() const
    {
        if (!_native)
            return {};
        return skia::to(_native->getBounds());
    }

    void Region::_confirmNative()
    {
        if (!_native)
            _native = std::make_shared<SkRegion>();
    }
}
