#include "stdafx.h"
#include "Region.h"
#include "core/io/filestream.h"

#include "skia/skia.h"
#include <SkRegion.h>

namespace graphics
{
    using namespace core;

    Region::~Region()
    {
    }


    void Region::_confirmNative()
    {
        if (!_native)
            _native = std::make_shared<SkRegion>();
    }
}
