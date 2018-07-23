#pragma once

class SkTextBlob;

namespace graphics
{
    class TextBlob
    {
    public:
        TextBlob();
        ~TextBlob();

    public:
        std::shared_ptr<SkTextBlob> native_share() { return _native; }
        SkTextBlob & native() { return *_native; }
        const SkTextBlob & native() const { return *_native; }
    private:
        std::shared_ptr<SkTextBlob> _native = nullptr;
    };
}
