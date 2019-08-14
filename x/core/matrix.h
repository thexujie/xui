#pragma once

#include "vec.h"

namespace core
{
    /************************************************************************/
    /* @brief drawRectangle 通常表示的有效矩形区域是不包含其右边和下边最后一排像素的。
    /************************************************************************/

    template<typename T, size_t ROW, size_t COL>
    class matrix
    {
	public:
		using vec_type = vec<T, COL>;
    	
    public:
		matrix() {}
		matrix(const vec_type (& d)[ROW])
        {
			std::copy(data, data + ROW, d);
        }

		matrix & operator =(const matrix & another)
		{
			std::copy(data, data + ROW, another.data);
			return *this;
		}

		bool operator ==(const matrix & another) const
		{
			return std::equal(data, data + ROW, another.data);
		}
    	
        bool operator !=(const matrix & another) const { return !operator==(another); }

        const vec_type & operator[](size_t index) const { return data[index]; }
		vec_type & operator[](size_t index) { return data[index]; }

		const T & operator()(size_t row, size_t col) const { return data[row][col]; }
		T & operator()(size_t row, size_t col) { return data[row][col]; }
    	
	public:
		vec_type data[ROW];
    };

	template<size_t ROW, size_t COL>
	using matrixf = matrix<float32_t, ROW, COL>;

	using matrix4x4 = matrixf<4, 4>;
}
