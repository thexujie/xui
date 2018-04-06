#pragma once

#include "align.h"
#include "vec2.h"

namespace math
{
    /************************************************************************/
    /* @brief rectangle 通常表示的有效矩形区域是不包含其右边和下边最后一排像素的。
    /************************************************************************/

    template <typename RT>
    class rect_tmpl
    {
        public:
        rect_tmpl(): x(0), y(0), w(0), h(0) {}
        rect_tmpl(RT _x, RT _y, RT _w, RT _h)
                : x(_x), y(_y), w(_w), h(_h)
        {
        }
        //explicit rect_tmpl(const vec2<RT> & _size)
        //	: position(0, 0), size(_size)
        //{
        //}
        rect_tmpl(const vec2<RT> & _position, const vec2<RT> & _size)
                : position(_position), size(_size)
        {
        }
        rect_tmpl(const rect_tmpl & another)
                : x(another.x), y(another.y), w(another.w), h(another.h)
        {
        }

            const RT right() const{return x + w; }
            const RT bottom() const{return y + h; }

            vec2<RT > left_top() const{return vec2<RT>(x, y); }
            vec2<RT > right_top() const{return vec2<RT>(x + w, y); }
            vec2<RT > left_bottom() const{return vec2<RT>(x, y + h); }
            vec2<RT > right_bottom() const{return vec2<RT>(x + w, y + h); }

        rect_tmpl & operator =(const rect_tmpl & rect)
        {
            x = rect.x;
            y = rect.y;
            w = rect.w;
            h = rect.h;
                return *this;
            
        }

        rect_tmpl & operator =(const vec2<RT> & size)
        {
            x = 0;
            y = 0;
            w = size.x;
            h = size.y;
                return *this;
            
        }

            void set(RT _x, RT _y, RT _w, RT _h)
        {
            x = _x;
            y = _y;
            w = _w;
            h = _h;
            
        }

            void set(const vec2<RT> & _position, const vec2<RT> & _size)
        {
            x = _position.x;
            y = _position.y;
            w = _size.x;
            h = _size.y;
            
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

            bool is_empty() const{return w <= 0 || h <= 0; }
            void set_empty() {w = 0; h = 0; }
            bool is_valid() const{return w > 0 && h > 0; }
            rect_tmpl expand(RT expand) const
        {
                return rect_tmpl(x - expand, y - expand, w + expand * 2, h + expand * 2);
            
        }
            rect_tmpl expand(const RT & expandX, const RT & expandY) const
        {
                return rect_tmpl(x - expandX, y - expandY, w + expandX * 2, h + expandY * 2);
            
        }

            rect_tmpl intersect(const rect_tmpl & another) const
        {
                return operator &(another);
            
        }

            bool intersect(const rect_tmpl & another, rect_tmpl & result) const
        {
            result = operator &(another);
                return result.is_valid();
            
        }

        // 并集
            rect_tmpl join(const rect_tmpl<RT> another) const
        {
                return operator |(another);
            
        }

        // 偏移
            rect_tmpl offset(RT off_x, RT off_y) const
        {
                return rect_tmpl(x + off_x, y + off_y, w, h);
            
        }

            rect_tmpl operator +(const vec2<RT> & offset) const
        {
                return rect_tmpl(x + offset.x, y + offset.y, w, h);
            
        }

        rect_tmpl & operator +=(const vec2<RT> & offset)
        {
            x += offset.x;
            y += offset.y;
                return *this;
            
        }

            rect_tmpl operator -(const vec2<RT> & offset) const
        {
                return rect_tmpl(x - offset.x, y - offset.y, w, h);
            
        }

        rect_tmpl & operator -=(const vec2<RT> & offset)
        {
            x -= offset.x;
            y -= offset.y;
                return *this;
            
        }

            rect_tmpl operator |(const rect_tmpl & another) const
        {
                if (another.is_empty())
                    return *this;
                else if (is_empty())
                    return another;
                else
            {
                    RT x_min = minof(x, another.x);
                    RT x_max = maxof(right(), another.right());
                    RT y_min = minof(y, another.y);
                    RT y_max = maxof(bottom(), another.bottom());
                    return rect_tmpl(x_min, y_min, x_max - x_min, y_max - y_min);
                
            }
        }

        rect_tmpl & operator |=(const rect_tmpl & another)
        {
                if (!another.is_empty())
            {
                    if (is_empty())
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
                else {}
                return *this;
            
        }


            rect_tmpl operator &(const rect_tmpl & another) const
        {
                if (another.is_empty())
                    return *this;
                else if (is_empty())
                    return another;
                else
            {
                    RT x_min = x > another.x ? x : another.x;
                    RT x_max = right() < another.right() ? right() : another.right();
                    RT y_min = y > another.y ? y : another.y;
                    RT y_max = bottom() < another.bottom() ? bottom() : another.bottom();

                    if (x_max <= x_min || y_max <= y_min)
                        return rect_tmpl();
                    else
                        return rect_tmpl(x_min, y_min, x_max - x_min, y_max - y_min);
                
            }
        }

        rect_tmpl & operator &=(const rect_tmpl & another)
        {
                if (another.is_empty() || is_empty())
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

            bool operator ==(const rect_tmpl & another) const
        {
                return x == another.x && y == another.y && w == another.w && h == another.h;
            
        }
            bool operator !=(const rect_tmpl & another) const{return !operator==(another); }

            template<typename RT2 > 
            explicit operator rect_tmpl<RT2>() const
        {
                return rect_tmpl<RT2>((RT2)x, (RT2)y, (RT2)w, (RT2)h);
            
        }

        public:
            union
        {
                RT at[4];
                struct
            {
                //! 矩形的位置。
                    vec2<RT > position;
                //! 矩形的大小。
                    vec2<RT > size;
                
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
                //! 矩形的宽度。
                    RT w;
                //! 矩形的高度。
                    RT h;
                
            };
            
        };
        
    };
}
