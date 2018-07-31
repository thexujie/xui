#pragma once

#include "align.h"
#include "vec2.h"
namespace core
{
    /************************************************************************/
    /* @brief drawRectangle 通常表示的有效矩形区域是不包含其右边和下边最后一排像素的。
    /************************************************************************/

    template<typename T>
    class vec4
    {
    public:
        vec4() : x(), y(), cx(), cy() { }

        vec4(T _t)
            : x(_t), y(_t), cx(_t), cy(_t) { }

        vec4(T _x, T _y)
            : x(_x), y(_y), cx(_x), cy(_y) { }

        vec4(T _x, T _y, T _w, T _h)
            : x(_x), y(_y), cx(_w), cy(_h) { }

        vec4(const vec2<T> & _pos, const vec2<T> & _size)
            : pos(_pos), size(_size) { }

        vec4(const vec4 & another)
            : x(another.x), y(another.y), cx(another.cx), cy(another.cy) { }

        T left() const { return x; }
        T top() const { return y; }
        T right() const { return x + cx; }
        T bottom() const { return y + cy; }

        const T centerX() const { return x + cx / 2; }
        const T centerY() const { return y + cy / 2; }
        void setLeft(T val) { x = val; }
        void setTop(T val) { y = val; }
        void setRight(T val) { cx = val - x; }
        void setBottom(T val) { cy = val - y; }
        void moveLeft(T val) { x += val; }
        void moveTop(T val) { y += val; }
        void moveRight(T val) { cx += val; }
        void moveBottom(T val) { cy += val; }

        vec2<T> leftTop() const { return vec2<T>(x, y); }
        vec2<T> rightTop() const { return vec2<T>(x + cx, y); }
        vec2<T> leftBottom() const { return vec2<T>(x, y + cy); }
        vec2<T> rightBottom() const { return vec2<T>(x + cx, y + cy); }
        vec2<T> leftCenter() const { return vec2<T>(x, y + cy / 2); }
        vec2<T> topCenter() const { return vec2<T>(x + cx / 2, y); }
        vec2<T> rightCenter() const { return vec2<T>(x + cx, y + cy / 2); }
        vec2<T> bottomCenter() const { return vec2<T>(x + cx / 2, y + cy); }
        vec2<T> center() const { return vec2<T>(x + cx / 2, y + cy / 2); }

        vec2<T> bleftTop() const { return vec2<T>(bleft, btop); }
        vec2<T> brightTop() const { return vec2<T>(bright, btop); }
        vec2<T> brightBottom() const { return vec2<T>(bright, bbottom); }
        vec2<T> bleftBottom() const { return vec2<T>(bleft, bbottom); }
        T bwidth() const { return bleft + bright; }
        T bheight() const { return btop + bbottom; }
        vec2<T> bsize() const { return vec2<T>(bleft + bright, btop + bbottom); }

        vec4 & operator =(const vec4 & rect)
        {
            x = rect.x;
            y = rect.y;
            cx = rect.cx;
            cy = rect.cy;
            return *this;
        }

        vec4 & operator =(const vec2<T> & size)
        {
            x = 0;
            y = 0;
            cx = size.x;
            cy = size.y;
            return *this;
        }

        void set(T _x, T _y, T _w, T _h)
        {
            x = _x;
            y = _y;
            cx = _w;
            cy = _h;
        }

        void set(const vec2<T> & _position, const vec2<T> & _size)
        {
            x = _position.x;
            y = _position.y;
            cx = _size.x;
            cy = _size.y;
        }

        bool is_point_in(const T & _x, const T & _y) const
        {
            return _x >= x && _y >= y &&
                _x < right() && _y < bottom();
        }

        bool is_point_in(const vec2<T> & point) const
        {
            return is_point_in(point.x, point.y);
        }

        bool empty() const
        {
            if (std::is_floating_point<T>::value)
                return std::abs(cx) < std::numeric_limits<T>::epsilon() || std::abs(cy) < std::numeric_limits<T>::epsilon();
            else
                return cx <= 0 || cy <= 0;
        }

        void set_empty()
        {
            cx = 0;
            cy = 0;
        }

        vec4 expand(T expand) const
        {
            return vec4(x - expand, y - expand, cx + expand * 2, cy + expand * 2);
        }

        vec4 expand(const vec4 & bound) const
        {
            return vec4(x - bound.bleft, y - bound.btop, cx + bound.bleft + bound.bright, cy + bound.btop + bound.bbottom);
        }

        vec4 expand(const T & expandX, const T & expandY) const
        {
            return vec4(x - expandX, y - expandY, cx + expandX * 2, cy + expandY * 2);
        }

        vec4 intersect(const vec4 & another) const
        {
            return operator &(another);
        }

        bool intersect(const vec4 & another, vec4 & result) const
        {
            result = operator &(another);
            return !result.empty();
        }

        // 并集
        vec4 unite(const vec4<T> another) const
        {
            return operator |(another);
        }

        // 偏移
        vec4 offset(T off_x, T off_y) const
        {
            return vec4(x + off_x, y + off_y, cx, cy);
        }


        //------------------------------------------------------- 四则运算
        vec4 operator +(const vec4 & vec) const
        {
            return vec4(x + vec.x, y + vec.y, z + vec.z, w + vec.w);
        }

        vec4 operator -(const vec4 & vec) const
        {
            return vec4(x - vec.x, y - vec.y, z - vec.z, w - vec.w);
        }

        vec4 operator *(const vec4 & vec) const
        {
            return vec4(x * vec.x, y * vec.y, z * vec.z, w * vec.w);
        }

        vec4 operator /(const vec4 & vec) const
        {
            return vec4(x / vec.x, y / vec.y, z / vec.z, w / vec.w);
        }

        vec4 & operator +=(const vec4 & vec)
        {
            x += vec.x;
            y += vec.y;
            z += vec.z;
            w += vec.w;
            return *this;
        }

        vec4 & operator -=(const vec4 & vec)
        {
            x -= vec.x;
            y -= vec.y;
            z -= vec.z;
            w -= vec.w;
            return *this;
        }

        vec4 & operator *=(const vec4 & vec)
        {
            x *= vec.x;
            y *= vec.y;
            z *= vec.z;
            w *= vec.w;
            return *this;
        }

        vec4 & operator /=(const vec4 & vec)
        {
            x /= vec.x;
            y /= vec.y;
            z /= vec.z;
            w /= vec.w;
            return *this;
        }

        vec4 operator *(const T & rate) const
        {
            return vec4(x * rate, y * rate, z * rate, w * rate);
        }

        vec4 operator /(const T & rate)
        {
            return vec4(x / rate, y / rate, z / rate, w / rate);
        }

        vec4 & operator *=(const T & rate)
        {
            x *= rate;
            y *= rate;
            z *= rate;
            w *= rate;
            return *this;
        }

        vec4 & operator /=(const T & rate)
        {
            x /= rate;
            y /= rate;
            z /= rate;
            w /= rate;
            return *this;
        }

        vec4 operator -() const
        {
            return vec4(-x, -y, -z, -w);
        }

        vec4 operator +() const
        {
            return *this;
        }

        //--------------------------------------------------比较运算
        vec4 operator +(const vec2<T> & offset) const
        {
            return vec4(x + offset.x, y + offset.y, cx, cy);
        }

        vec4 & operator +=(const vec2<T> & offset)
        {
            x += offset.x;
            y += offset.y;
            return *this;
        }

        vec4 operator -(const vec2<T> & offset) const
        {
            return vec4(x - offset.x, y - offset.y, cx, cy);
        }

        vec4 & operator -=(const vec2<T> & offset)
        {
            x -= offset.x;
            y -= offset.y;
            return *this;
        }

        vec4 operator |(const vec4 & another) const
        {
            if (another.empty())
                return *this;
            else if (empty())
                return another;
            else
            {
                T x_min = minof(x, another.x);
                T x_max = maxof(right(), another.right());
                T y_min = minof(y, another.y);
                T y_max = maxof(bottom(), another.bottom());
                return vec4(x_min, y_min, x_max - x_min, y_max - y_min);
            }
        }

        vec4 & operator |=(const vec4 & another)
        {
            if (!another.empty())
            {
                if (empty())
                    * this = another;
                else
                {
                    T x_min = minof(x, another.x);
                    T x_max = maxof(right(), another.right());
                    T y_min = minof(y, another.y);
                    T y_max = maxof(bottom(), another.bottom());
                    set(x_min, y_min, x_max - x_min, y_max - y_min);
                }
            }
            else {}
            return *this;
        }


        vec4 operator &(const vec4 & another) const
        {
            if (another.empty())
                return *this;
            else if (empty())
                return another;
            else
            {
                T x_min = x > another.x ? x : another.x;
                T x_max = right() < another.right() ? right() : another.right();
                T y_min = y > another.y ? y : another.y;
                T y_max = bottom() < another.bottom() ? bottom() : another.bottom();

                if (x_max <= x_min || y_max <= y_min)
                    return vec4();
                else
                    return vec4(x_min, y_min, x_max - x_min, y_max - y_min);
            }
        }

        vec4 & operator &=(const vec4 & another)
        {
            if (another.empty() || empty())
                set_empty();
            else
            {
                T x_min = x > another.x ? x : another.x;
                T x_max = right() < another.right() ? right() : another.right();
                T y_min = y > another.y ? y : another.y;
                T y_max = bottom() < another.bottom() ? bottom() : another.bottom();

                if (x_max <= x_min || y_max <= y_min)
                    set_empty();
                else
                    set(x_min, y_min, x_max - x_min, y_max - y_min);
            }
            return *this;
        }

        bool operator ==(const vec4 & another) const
        {
            if (empty() && another.empty())
                return true;

            if (empty() || another.empty())
                return false;

            if (std::is_floating_point<T>::value)
                return std::abs(x - another.x) < std::numeric_limits<T>::epsilon() &&
                std::abs(y - another.y) < std::numeric_limits<T>::epsilon() &&
                std::abs(cx - another.cx) < std::numeric_limits<T>::epsilon() &&
                std::abs(cy - another.cy) < std::numeric_limits<T>::epsilon();
            else
                return x == another.x && y == another.y && cx == another.cx && cy == another.cy;
        }
        bool operator !=(const vec4 & another) const { return !operator==(another); }

        const T & operator[](int32_t index) const { return arr[index % 4]; }
         T & operator[](int32_t index) { return arr[index % 4]; }

        template<typename T2>
        vec4<T2> to() const
        {
            return { static_cast<T2>(x), static_cast<T2>(y), static_cast<T2>(cx), static_cast<T2>(cy) };
        }

        template<class ConvertT>
        auto convert(ConvertT convert) const
        {
            return vec4(convert(x), convert(y), convert(cx), convert(cy));
        }

    public:
        union
        {
            struct
            {
                //! 矩形的位置。
                vec2<T> pos;
                //! 矩形的大小。
                vec2<T> size;
            };

            struct
            {
                //! 矩形的位置。
                vec2<T> xy;
                //! 矩形的大小。
                vec2<T> zw;
            };

            struct
            {
                union
                {
                    struct
                    {
                        //! 矩形的横坐标。
                        T x;
                        //! 矩形的纵坐标。
                        T y;
                    };

                    struct
                    {
                        //! 左边界。
                        T bleft;
                        //! 上边界。
                        T btop;
                    };
                };

                union
                {
                    struct
                    {
                        T z;
                        T w;
                    };

                    struct
                    {
                        // 右边界
                        T bright;
                        // 下边界
                        T bbottom;
                    };

                    struct
                    {
                        //! 矩形的宽度。
                        T cx;
                        //! 矩形的高度。
                        T cy;
                    };

                    struct
                    {
                        //! 矩形的宽度。
                        T width;
                        //! 矩形的高度。
                        T height;
                    };
                };
            };

            std::array<T, 4> arr;
        };
    };

    typedef vec4<int32_t> rc32i;
    typedef vec4<float32_t> rc32f;
    typedef vec4<int32_t> vec4i;
    typedef vec4<float32_t> vec4f;
}
