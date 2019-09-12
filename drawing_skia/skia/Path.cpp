#include "stdafx.h"
#include "Path.h"
#include "core/io/filestream.h"


namespace drawing::skia
{
    using namespace core;

    Path::~Path()
    {
    }

    void Path::clear()
    {
        if (_native)
            _native->reset();
    }

    void Path::close()
    {
        if (_native)
            _native->close();
    }

    void Path::move(const core::pointf & pos)
    {
        _confirmNative();
        _native->moveTo(skia::from(pos));
    }

    void Path::line(const core::pointf & pos)
    {
        _confirmNative();
        _native->lineTo(skia::from(pos));
    }

    void Path::arc(const core::pointf & pos0, const core::pointf & po1, float32_t radius)
    {
        _confirmNative();
        _native->arcTo(skia::from(pos0), skia::from(po1), radius);
    }

    void Path::transform(const core::float3x2 & matrix)
    {
        _confirmNative();
        _native->transform(skia::from(matrix));
    }

    void Path::transform(const core::float3x2 & matrix, Path & output)
    {
        _confirmNative();
        output._confirmNative();
        _native->transform(skia::from(matrix), output.native_ptr());
    }

    void Path::_confirmNative()
    {
        if (!_native)
            _native = std::make_shared<SkPath>();
    }

    //core::rectf Path::computeTightBounds() const
    //{
    //    return skia::to(_native->computeTightBounds());
    //}
}
