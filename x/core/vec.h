#pragma once

#include "align.h"
#include "vec2.h"

namespace core
{
    template<typename T, size_t SIZE>
    class vec
    {
    public:
		vec() = default;

        explicit vec(T t)
        {
			std::fill(data, data + SIZE, t);
        }

		vec(const T (&t)[SIZE])
		{
			std::copy(data, data + SIZE, t, t + SIZE);
		}

        vec(const vec & another)
		{
			std::copy(data, data + SIZE, another.data, another.data + SIZE);
		}

        vec & operator =(const vec & another)
        {
			std::copy(data, data + SIZE, another.data, another.data + SIZE);
            return *this;
        }

        void set(const vec & another)
        {
			std::copy(data, data + SIZE, another.data, another.data + SIZE);
        }

        void set(T t)
        {
			std::fill(data, data + SIZE, t);
        }

		void set(const T(&t)[SIZE])
		{
			std::copy(data, data + SIZE, t, t + SIZE);
		}

        bool operator ==(const vec & another) const
        {
			if constexpr (std::is_floating_point_v<T>)
				return std::equal(data, data + SIZE, another.data, [](const T & a, const T & b) {std::abs(a - b) < std::numeric_limits<T>::epsilon(); });
			else
				return std::equal(data, data + SIZE, another.data);
        }
        bool operator !=(const vec & another) const { return !operator==(another); }

        const T & operator[](size_t index) const { return data[index]; }
         T & operator[](size_t index) { return data[index]; }

    public:
		T data[SIZE];
    };
}
