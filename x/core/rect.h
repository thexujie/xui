#pragma once

#include "align.h"
#include "vec2.h"

namespace core
{
    /************************************************************************/
    /* @brief drawRectangle 通常表示的有效矩形区域是不包含其右边和下边最后一排像素的。
    /************************************************************************/

    template<typename RT>
    class rect
    {
    public:
        rect() : x(0), y(0), cx(0), cy(0) { }

        rect(RT _x, RT _y, RT _w, RT _h)
            : x(_x), y(_y), cx(_w), cy(_h) { }

        //explicit rect_tmpl(const vec2<RT> & _size)
        //	: position(0, 0), size(_size)
        //{
        //}
        rect(const vec2<RT> & _position, const vec2<RT> & _size)
            : position(_position), size(_size) { }

        rect(const rect & another)
            : x(another.x), y(another.y), cx(another.cx), cy(another.cy) { }

        const RT right() const { return x + cx; }
        const RT bottom() const { return y + cy; }
        const RT centerX() const { return x + cx / 2; }
        const RT centerY() const { return y + cy / 2; }
        void setLeft(RT val) { x = val; }
        void setTop(RT val) { y = val; }
        void setRight(RT val) { cx = val - x; }
        void setBottom(RT val) { cy = val - y; }
        void moveLeft(RT val) { x += val; }
        void moveTop(RT val) { y += val; }
        void moveRight(RT val) { cx += val; }
        void moveBottom(RT val) { cy += val; }

        vec2<RT> leftTop() const { return vec2<RT>(x, y); }
        vec2<RT> rightTop() const { return vec2<RT>(x + cx, y); }
        vec2<RT> leftBottom() const { return vec2<RT>(x, y + cy); }
        vec2<RT> rightBottom() const { return vec2<RT>(x + cx, y + cy); }
        vec2<RT> leftCenter() const { return vec2<RT>(x, y + cy / 2); }
        vec2<RT> topCenter() const { return vec2<RT>(x + cx / 2, y); }
        vec2<RT> rightCenter() const { return vec2<RT>(x + cx, y + cy / 2); }
        vec2<RT> bottomCenter() const { return vec2<RT>(x + cx / 2, y + cy); }
        vec2<RT> center() const { return vec2<RT>(x + cx / 2, y + cy / 2); }

        rect & operator =(const rect & rect)
        {
            x = rect.x;
            y = rect.y;
            cx = rect.cx;
            cy = rect.cy;
            return *this;
        }

        rect & operator =(const vec2<RT> & size)
        {
            x = 0;
            y = 0;
            cx = size.x;
            cy = size.y;
            return *this;
        }

        void set(RT _x, RT _y, RT _w, RT _h)
        {
            x = _x;
            y = _y;
            cx = _w;
            cy = _h;
        }

        void set(const vec2<RT> & _position, const vec2<RT> & _size)
        {
            x = _position.x;
            y = _position.y;
            cx = _size.x;
            cy = _size.y;
        }

        bool is_point_in(const RT & _x, const RT & _y) const
        {
            return _x >= x && _y >= y &&
                _x < right() && _y < bottom();
        }

        bool is_point_in(const vec2<RT> & point) const
        {
            return is_point_in(point.x, point.y);
        }

        bool empty() const
        {
            if (std::is_floating_point<RT>::value)
                return std::abs(cx) < std::numeric_limits<RT>::epsilon() || std::abs(cy) < std::numeric_limits<RT>::epsilon();
            else
                return cx <= 0 || cy <= 0;
        }

        void set_empty()
        {
            cx = 0;
            cy = 0;
        }

        rect expand(RT expand) const
        {
            return rect(x - expand, y - expand, cx + expand * 2, cy + expand * 2);
        }

        rect expand(const RT & expandX, const RT & expandY) const
        {
            return rect(x - expandX, y - expandY, cx + expandX * 2, cy + expandY * 2);
        }

        rect intersect(const rect & another) const
        {
            return operator &(another);
        }

        bool intersect(const rect & another, rect & result) const
        {
            result = operator &(another);
            return !result.empty();
        }

        // 并集
        rect join(const rect<RT> another) const
        {
            return operator |(another);
        }

        // 偏移
        rect offset(RT off_x, RT off_y) const
        {
            return rect(x + off_x, y + off_y, cx, cy);
        }

        rect operator +(const vec2<RT> & offset) const
        {
            return rect(x + offset.x, y + offset.y, cx, cy);
        }

        rect & operator +=(const vec2<RT> & offset)
        {
            x += offset.x;
            y += offset.y;
            return *this;
        }

        rect operator -(const vec2<RT> & offset) const
        {
            return rect(x - offset.x, y - offset.y, cx, cy);
        }

        rect & operator -=(const vec2<RT> & offset)
        {
            x -= offset.x;
            y -= offset.y;
            return *this;
        }

        rect operator |(const rect & another) const
        {
            if (another.empty())
                return *this;
            else if (empty())
                return another;
            else
            {
                RT x_min = minof(x, another.x);
                RT x_max = maxof(right(), another.right());
                RT y_min = minof(y, another.y);
                RT y_max = maxof(bottom(), another.bottom());
                return rect(x_min, y_min, x_max - x_min, y_max - y_min);
            }
        }

        rect & operator |=(const rect & another)
        {
            if (!another.empty())
            {
                if (empty())
                    * this = another;
                else
                {
                    RT x_min = minof(x, another.x);
                    RT x_max = maxof(right(), another.right());
                    RT y_min = minof(y, another.y);
                    RT y_max = maxof(bottom(), another.bottom());
                    set(x_min, y_min, x_max - x_min, y_max - y_min);
                }
            }
            else { }
            return *this;
        }


        rect operator &(const rect & another) const
        {
            if (another.empty())
                return *this;
            else if (empty())
                return another;
            else
            {
                RT x_min = x > another.x ? x : another.x;
                RT x_max = right() < another.right() ? right() : another.right();
                RT y_min = y > another.y ? y : another.y;
                RT y_max = bottom() < another.bottom() ? bottom() : another.bottom();

                if (x_max <= x_min || y_max <= y_min)
                    return rect();
                else
                    return rect(x_min, y_min, x_max - x_min, y_max - y_min);
            }
        }

        rect & operator &=(const rect & another)
        {
            if (another.empty() || empty())
                set_empty();
            else
            {
                RT x_min = x > another.x ? x : another.x;
                RT x_max = right() < another.right() ? right() : another.right();
                RT y_min = y > another.y ? y : another.y;
                RT y_max = bottom() < another.bottom() ? bottom() : another.bottom();

                if (x_max <= x_min || y_max <= y_min)
                    set_empty();
                else
                    set(x_min, y_min, x_max - x_min, y_max - y_min);
            }
            return *this;
        }

        bool operator ==(const rect & another) const
        {
            if (empty() && another.empty())
                return true;

            if (empty() || another.empty())
                return false;

            if (std::is_floating_point<RT>::value)
                return std::abs(x - another.x) < std::numeric_limits<RT>::epsilon() &&
                std::abs(y - another.y) < std::numeric_limits<RT>::epsilon() &&
                std::abs(cx - another.cx) < std::numeric_limits<RT>::epsilon() &&
                std::abs(cy - another.cy) < std::numeric_limits<RT>::epsilon();
            else
                return x == another.x && y == another.y && cx == another.cx && cy == another.cy;
        }

        bool operator !=(const rect & another) const { return !operator==(another); }

        template<typename RT2>
        explicit operator rect<RT2>() const
        {
            return rect<RT2>((RT2)x, (RT2)y, (RT2)cx, (RT2)cy);
        }

    public:
        union
        {
            RT at[4];

            struct
            {
                //! 矩形的位置。
                vec2<RT> position;
                //! 矩形的大小。
                vec2<RT> size;
            };

            struct
            {
                union
                {
                    struct
                    {
                        //! 矩形的横坐标。
                        RT x;
                        //! 矩形的纵坐标。
                        RT y;
                    };

                    struct
                    {
                        //! 矩形的横坐标。
                        RT left;
                        //! 矩形的纵坐标。
                        RT top;
                    };
                };

                union
                {
                    struct
                    {
                        //! 矩形的宽度。
                        RT cx;
                        //! 矩形的高度。
                        RT cy;
                    };

                    struct
                    {
                        //! 矩形的宽度。
                        RT width;
                        //! 矩形的高度。
                        RT height;
                    };
                };
            };
        };
    };

    typedef rect<int32_t> rc32i;
    typedef rect<float32_t> rc32f;
}
