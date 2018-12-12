#pragma once

class SkTextBlob;

namespace drawing
{
    class TextBlob
    {
    public:
        TextBlob();
        TextBlob(std::shared_ptr<SkTextBlob> native, core::sizef size);
        TextBlob(std::string str, const StringFormat & format);
        TextBlob(std::string str, float32_t width, const StringFormat & format);
        ~TextBlob();

        const core::sizef & size() const { return _size; }
        void reset(std::string str, float32_t width, const StringFormat & format);

    public:
        void setNative(std::shared_ptr<SkTextBlob> blob, core::sizef size) { _native = blob; _size = size; }
        std::shared_ptr<SkTextBlob> native_shared() const { return _native; }
        SkTextBlob & native() { return *_native; }
        const SkTextBlob & native() const { return *_native; }
        SkTextBlob * native_ptr() { return _native.get(); }
        const SkTextBlob * native_ptr() const { return _native.get(); }
    private:
        std::shared_ptr<SkTextBlob> _native = nullptr;
        core::sizef _size;
    };
}
