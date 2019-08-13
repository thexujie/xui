#pragma once

class SkBitmap;
class SkSurface;

namespace drawing
{
    class GraphicsDevice
    {
    public:
        virtual ~GraphicsDevice() {}
        virtual bitmap_buffer buffer() const = 0;
        virtual core::sizei size() const = 0;
        virtual void * hdc() const = 0;
        virtual core::error Save(std::string path, core::image_type type = core::image_type_none, int32_t quality = 100) = 0;
    };

    class Bitmap : public GraphicsDevice
    {
    public:
        Bitmap() = default;
        ~Bitmap() = default;

        Bitmap(const core::sizei & size);

        operator bool() const { return !!_native; }
        bitmap_buffer buffer() const override;
        core::sizei size() const override;
        void * hdc() const override { return nullptr; }
        core::error Save(std::string path, core::image_type type = core::image_type_none, int32_t quality = 100);

        std::shared_ptr<SkBitmap> native_shared() { return _native; }
        SkBitmap & native() { return *_native; }
        const SkBitmap & native() const { return *_native; }
        SkBitmap * native_ptr() { return _native.get(); }
        const SkBitmap * native_ptr() const { return _native.get(); }
    private:
        core::error _state = core::ok;
        std::shared_ptr<SkBitmap> _native = nullptr;
        core::sizei _size;
    };

    class Surface : public GraphicsDevice
    {
    public:
        Surface() = default;
        Surface(const core::sizei & size);
        ~Surface();

        core::error resize(const core::sizei & size);

        operator bool() const { return !!_native; }
        bitmap_buffer buffer() const override;
        core::sizei size() const override;
        core::error Save(std::string path, core::image_type type = core::image_type_none, int32_t quality = 100) override;

        std::shared_ptr<SkSurface> native_shared() { return _native; }
        SkSurface & native() { return *_native; }
        const SkSurface & native() const { return *_native; }
        SkSurface * native_ptr() { return _native.get(); }
        const SkSurface * native_ptr() const { return _native.get(); }
    private:
        core::error _state = core::ok;
        std::shared_ptr<SkSurface> _native = nullptr;
        core::sizei _size;

#ifdef _WIN32
        void * _data = nullptr;
        int32_t _strike = 0;
        int32_t _pitch = 0;
        void * _hdc = nullptr;
        void * _bmp = nullptr;
    public:
        void * hdc() const { return _hdc; }
        void * bmp() const { return _bmp; }
#endif
    };
}
