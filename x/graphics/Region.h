#pragma once
#include "GraphicsService.h"

class SkRegion;

namespace graphics
{
    class Region final
    {
    public:
        Region() = default;
        ~Region();

    private:
        void _confirmNative();

    public:
        operator bool() const { return !!_native; }
        std::shared_ptr<SkRegion> native_shared() { return _native; }
        SkRegion & native() { return *_native; }
        const SkRegion & native() const { return *_native; }
        SkRegion * native_ptr() { return _native.get(); }
        const SkRegion * native_ptr() const { return _native.get(); }
    private:
        std::shared_ptr<SkRegion> _native;
    };
}
