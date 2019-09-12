#pragma once

#include "skia.h"

class SkPath;

namespace skia
{
    class Path : public drawing::Path
    {
    public:
        Path() = default;
        ~Path();

        void clear();
        void close();
        void move(const core::pointf & pos);
        void line(const core::pointf & pos);
        void arc(const core::pointf & pos0, const core::pointf & po1, float32_t radius);

        void transform(const core::float3x2 & matrix);
        void transform(const core::float3x2 & matrix, Path & output);

        template<class Iter>
        void fromPoints(Iter iter, const Iter end, bool bclose)
        {
            if (iter == end)
                return;

            int32_t num = 0;
            move(*iter);
            iter++;
            while(iter != end)
            {
                line(*iter);
                iter++;
                ++num;
            }

            if (bclose && num > 2)
                close();
        }

    private:
        void _confirmNative();

    public:
        operator bool() const { return !!_native; }
        std::shared_ptr<SkPath> native_shared() { return _native; }
        SkPath & native() { return *_native; }
        const SkPath & native() const { return *_native; }
        SkPath * native_ptr() { return _native.get(); }
        const SkPath * native_ptr() const { return _native.get(); }
    private:
        std::shared_ptr<SkPath> _native;
    };
}
