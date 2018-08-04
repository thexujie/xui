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

        void clear();
        bool empty() const;

        void setRect(const core::rc32i & rect);
        void addRect(const core::rc32i & rect);

        bool intersects(const core::rc32i & rect)const;
        bool intersects(const Region & region)const;

        bool contains(const core::pt32i & point) const;
        bool contains(const core::rc32i & rect) const;
        bool contains(const Region & region) const;

        core::rc32i bounds() const;
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
