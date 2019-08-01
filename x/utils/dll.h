#pragma once

namespace utils
{
    class dll
    {
    public:
        dll();
        dll(const std::u8string & path);
        ~dll();

        core::error load(const std::u8string & path, bool userDir = false);
        void release();
        operator bool() const { return !!_handle; }

		void * proc(const char * name);
		void * proc(const std::u8string & name);

		template<typename T>
		T get(const char * name)
		{
			void * ptr = proc(name);
			return static_cast<T>(ptr);
		}

        template<typename T>
        T get(const std::u8string & name)
        {
            void * ptr = proc(name);
            return static_cast<T>(ptr);
        }

        void * handle() const { return _handle; }

    protected:
        void * _handle = nullptr;
        std::u8string _path;
    };
}

