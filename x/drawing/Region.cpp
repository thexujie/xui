#include "stdafx.h"
#include "Region.h"
#include "core/io/filestream.h"

#include "skia/skia.h"
#include <SkRegion.h>

namespace drawing
{
    using namespace core;

    Region::Region(const Region & another)
    {
        _native = another._native;
    }

    Region::~Region() { }

    void Region::clear()
    {
        if (_native)
            _native->setEmpty();
    }

    bool Region::empty() const
    {
        if (!_native)
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
		if(rect.empty())
			return;

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


    RegionIterator::RegionIterator(const Region & region)
    {
        if(region)
            _native = std::make_shared<SkRegion::Iterator>(region.native());
    }

    bool RegionIterator::done()
    {
        if (!_native)
            return true;
        auto sri = std::reinterpret_pointer_cast<SkRegion::Iterator>(_native);
        return sri->done();
    }
    void  RegionIterator::next()
    {
        if (!_native)
            return;
        auto sri = std::reinterpret_pointer_cast<SkRegion::Iterator>(_native);
        sri->next();
    }

    core::rc32i RegionIterator::rect() const
    {
        if (!_native)
            return {};
        auto sri = std::reinterpret_pointer_cast<SkRegion::Iterator>(_native);
        return skia::to(sri->rect());
    }
}
