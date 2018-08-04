#pragma once

namespace core
{
    template<typename T>
    class attribute
    {
    public:
        attribute() : value() {}
        attribute(const T & value_) : value(value_) {}
        ~attribute() {}

        T & get() { return value; }
        const T & get() const{ return value; }
        void set(const T & value_) { value = value_; }

        attribute & operator = (const T & value_)
        {
            _expired = !(value_ == value);
            _available = true;
            value = value_;
            return *this;
        }

        operator T & () { return value; }
        operator const T & () const { return value; }
        operator bool() const { return _available; }
        bool available() const { return _available; }
        bool expired() const { return _expired; }

        bool operator == (const T & value_) const { return value == value_; }
        bool operator != (const T & value_) const { return value != value_; }
        bool operator > (const T & value_) const { return value > value_; }
        bool operator < (const T & value_) const { return value < value_; }
        bool operator >= (const T & value_) const { return value >= value_; }
        bool operator <= (const T & value_) const { return value <= value_; }

        T value;
    private:
        bool _available = false;
        bool _expired = false;
    };
}
