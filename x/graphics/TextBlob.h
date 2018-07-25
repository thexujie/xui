#pragma once
#include "StringFormat.h"

class SkTextBlob;

namespace graphics
{
    class TextBlob
    {
    public:
        TextBlob();
        TextBlob(std::string str, int32_t width, const StringFormat & format);
        ~TextBlob();

        void reset(std::string str, int32_t width, const StringFormat & format);

    public:
        std::shared_ptr<SkTextBlob> native_shared() const { return _native; }
        SkTextBlob & native() { return *_native; }
        const SkTextBlob & native() const { return *_native; }
        SkTextBlob * native_ptr() { return _native.get(); }
        const SkTextBlob * native_ptr() const { return _native.get(); }
    private:
        std::shared_ptr<SkTextBlob> _native = nullptr;
    };
}