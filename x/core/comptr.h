#pragma once

namespace core
{
    template<typename UT>
    UT * com_ref(UT * ptr)
    {
        if (!ptr)
            return ptr;
        ptr->AddRef();
        return ptr;
    }

    template<typename UT>
    void com_release(UT * ptr)
    {
        if (!ptr)
            return;
        ptr->Release();
    }

    template<typename UT>
    class comptr
    {
	public:
		using value_type = UT;

    public:
        comptr() {}
        comptr(std::nullptr_t)
        {
        }

        comptr(const comptr<UT> & another)
        {
            if (another._ptr)
            {
                _ptr = another._ptr;
                if (_ptr)
                    _ptr->AddRef();
            }
        }

        template<typename T>
        comptr(const comptr<T> & another)
        {
            if (static_cast<void *>(_ptr) != static_cast<void *>(another.get()))
            {
                if (_ptr)
                    _ptr->Release();

                if (another.get())
                    another.get()->QueryInterface(__uuidof(UT), static_cast<void **>(& _ptr));
            }
        }

        explicit comptr(UT * ptr) :_ptr(ptr)
        {

        }
        ~comptr()
        {
            if (_ptr)
            {
                _ptr->Release();
                _ptr = nullptr;
            }
        }

        UT * operator ->() const
        {
            if (!_ptr)
                throw std::exception("null com ptr");
            return _ptr;
        }

        operator bool() const { return !!_ptr; }

        comptr<UT> & operator=(const comptr<UT> & another)
        {
            if (_ptr == another._ptr)
                return *this;

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

        template<typename UT2>
        comptr<UT2> as() const
        {
            if (!_ptr)
                return nullptr;

            UT2 * p2 = nullptr;
            _ptr->QueryInterface(__uuidof(UT2), reinterpret_cast<void **>(&p2));
            return comptr<UT2>(p2);
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
            if (_ptr == ptr)
                return;

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
		UT ** getpp() const { if (_ptr) throw 1;  return const_cast<UT * *>(&_ptr); }
		UT ** getpp_safe() const { if (_ptr) { const_cast<comptr *>(this)->_ptr->Release(); const_cast<comptr *>(this)->_ptr = nullptr; }  return const_cast<UT * *>(&_ptr); }
        void ** getvv() const { if (_ptr) throw 1;  return reinterpret_cast<void **>(const_cast<UT **>(&_ptr)); }
        template<typename UT2>
        UT2 ** gettt() const { if (_ptr) throw 1;  return reinterpret_cast<UT2 **>(getvv()); }

        UT * ref() const
        {
            if (_ptr)
                _ptr->AddRef();
            return _ptr;
        }

        bool operator == (const comptr<UT> & another) const
        {
            return _ptr == another._ptr;
        }

        bool operator != (const comptr<UT> & another) const
        {
            return _ptr != another._ptr;
        }

        std::shared_ptr<UT> release_to_shared()
        {
            auto ptr = std::shared_ptr<UT>(_ptr, com_release<UT>);
            _ptr = nullptr;
            return ptr;
        }
    protected:
        UT * _ptr = nullptr;
    };
}
