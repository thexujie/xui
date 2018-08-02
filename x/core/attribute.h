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
            _na = false;
            value = value_;
            return *this;
        }

        operator T & () { return value; }
        operator const T & () const { return value; }
        operator bool() const { return !_na; }
        bool aviliable() const { return !_na; }

        bool operator == (const T & value_) const { return value == value_; }
        bool operator != (const T & value_) const { return value != value_; }
        bool operator > (const T & value_) const { return value > value_; }
        bool operator < (const T & value_) const { return value < value_; }
        bool operator >= (const T & value_) const { return value >= value_; }
        bool operator <= (const T & value_) const { return value <= value_; }

        T value;
    private:
        bool _na = true;
    };
}
