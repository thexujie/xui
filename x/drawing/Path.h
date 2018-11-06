#pragma once

class SkPath;

namespace drawing
{
    class path_source
    {
    public:
        virtual ~path_source() {}

        virtual void rewind(uint32_t path_id) = 0;
        virtual uint32_t vertex(float64_t * x, float64_t * y) = 0;
    };

    class Path final
    {
    public:
        Path() = default;
        ~Path();

        void clear();
        void close();
        void moveTo(const core::pt32f & pos);
        void lineTo(const core::pt32f & pos);
        void arcTo(const core::pt32f & pos0, const core::pt32f & po1, float32_t radius);

        void transform(const core::float3x2 & matrix);
        void transform(const core::float3x2 & matrix, Path & output);

        template<class Iter>
        void fromPoints(Iter iter, const Iter end, bool bclose)
        {
            if (iter == end)
                return;

            int32_t num = 0;
            moveTo(*iter);
            iter++;
            while(iter != end)
            {
                lineTo(*iter);
                iter++;
                ++num;
            }

            if (bclose && num > 2)
                close();
        }

        core::rectf  computeTightBounds() const;
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
