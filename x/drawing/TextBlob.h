#pragma once

class SkTextBlob;

namespace drawing
{
    class TextBlob
    {
    public:
        TextBlob();
        TextBlob(std::string str, const StringFormat & format);
        TextBlob(std::string str, float32_t width, const StringFormat & format);
        ~TextBlob();

        const core::si32f & size() const { return _size; }
        void reset(std::string str, float32_t width, const StringFormat & format);

    public:
        std::shared_ptr<SkTextBlob> native_shared() const { return _native; }
        SkTextBlob & native() { return *_native; }
        const SkTextBlob & native() const { return *_native; }
        SkTextBlob * native_ptr() { return _native.get(); }
        const SkTextBlob * native_ptr() const { return _native.get(); }
    private:
        std::shared_ptr<SkTextBlob> _native = nullptr;
        core::si32f _size;
    };
}
