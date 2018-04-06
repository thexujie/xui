#pragma once

namespace win32
{
    template<typename UT>
    class com_ptr
    {
    public:
        com_ptr() {}
        com_ptr(std::nullptr_t)
        {
        }

        com_ptr(const com_ptr<UT> & another)
        {
            if (another._ptr)
            {
                _ptr = another._ptr;
                if (_ptr)
                    _ptr->AddRef();
            }
        }

        template<typename T>
        com_ptr(const com_ptr<T> & another)
        {
            if ((void *)_ptr != (void *)another.get())
            {
                if (_ptr)
                    _ptr->Release();

                if (another.get())
                    another.get()->QueryInterface(__uuidof(UT), (void **)&_ptr);
            }
        }

        explicit com_ptr(UT * ptr) :_ptr(ptr) {}
        ~com_ptr()
        {
            if (_ptr)
                _ptr->Release();
        }

        UT ** operator & ()
        {
            return &_ptr;
        }

        UT * operator ->() const
        {
            return _ptr;
        }

        operator bool() const { return !!_ptr; }

        com_ptr<UT> & operator=(const com_ptr<UT> & another)
        {
            if (_ptr)
            {
                _ptr->Release();
                _ptr = nullptr;
            }

            if (another._ptr)
            {
                another._ptr->AddRef();
                _ptr = another._ptr;
            }
            return *this;
        }

        void reset()
        {
            if (_ptr)
            {
                _ptr->Release();
                _ptr = nullptr;
            }
        }

        void reset(UT * ptr)
        {
            if (_ptr)
            {
                _ptr->Release();
                _ptr = nullptr;
            }
            if (ptr)
                ptr->AddRef();
            _ptr = ptr;
        }

        UT * get() const { return _ptr; }
        UT * ref() const
        {
            if (_ptr)
                _ptr->AddRef();
            return _ptr;
        }

        bool operator == (const com_ptr<UT> & another) const
        {
            return _ptr == another._ptr;
        }

        bool operator != (const com_ptr<UT> & another) const
        {
            return _ptr != another._ptr;
        }

    protected:
        UT * _ptr = nullptr;
    };
}
