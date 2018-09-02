#pragma once

namespace core
{
    template<typename UT, void(*pfn_release)(UT * ptr)>
    class block_ptr
    {
    public:
        block_ptr() {}
        block_ptr(std::nullptr_t)
        {
        }

        block_ptr(const block_ptr & another) = delete;
        block_ptr(block_ptr && another) noexcept { reset(another.release()); }
        explicit block_ptr(UT * ptr) :_ptr(ptr) {}
        ~block_ptr()
        {
            if (_ptr)
            {
                pfn_release(_ptr);
                _ptr = nullptr;
            }
        }

        UT ** operator & ()
        {
            return &_ptr;
        }

        UT * operator ->() const
        {
            return _ptr;
        }

        void reset()
        {
            if (_ptr)
            {
                pfn_release(_ptr);
                _ptr = nullptr;
            }
        }

        UT * release()
        {
            UT * p = _ptr;
            _ptr = nullptr;
            return p;
        }

        operator bool() const { return !!_ptr; }

        block_ptr & operator=(const block_ptr & another)
        {
            reset();
            _ptr = another.release();
            return *this;
        }

        void reset(UT * ptr)
        {
            reset();
            _ptr = ptr;
        }

        UT * get() const { return _ptr; }

        bool operator == (const block_ptr & another) const
        {
            return _ptr == another._ptr;
        }

        bool operator != (const block_ptr & another) const
        {
            return _ptr != another._ptr;
        }

    protected:
        UT * _ptr = nullptr;
    };
}
