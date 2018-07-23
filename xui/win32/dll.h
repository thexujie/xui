#pragma once

namespace win32
{
    class dll
    {
    public:
        dll();
        dll(std::string path);
        ~dll();

        core::error load(std::string path);
        void free();
        operator bool() const { return !!_handle; }

        void * proc(std::string name);

        template<typename T>
        T get(std::string name)
        {
            void * ptr = proc(name);
            return static_cast<T>(ptr);
        }

    protected:
        void * _handle = nullptr;
    };
}

