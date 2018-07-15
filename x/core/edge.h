#pragma once

#include "vec2.h"

namespace core
{
    template<typename ValT>
    class edge_tmpl
    {
    public:
        edge_tmpl() : left(0), top(0), right(0), bottom(0) { }

        edge_tmpl(ValT _size) : left(_size), top(_size), right(_size), bottom(_size) { }

        edge_tmpl(ValT _left, ValT _top, ValT _right, ValT _bottom) : left(_left), top(_top), right(_right),
                                                                      bottom(_bottom) { }

        edge_tmpl(const edge_tmpl & another) : left(another.left), top(another.top), right(another.right),
                                               bottom(another.bottom) { }

        void set(ValT _size)
        {
            left = _size;
            top = _size;
            right = _size;
            bottom = _size;
        }

        void set(ValT _left, ValT _top, ValT _right, ValT _bottom)
        {
            left = _left;
            top = _top;
            right = _right;
            bottom = _bottom;
        }

        edge_tmpl & operator =(const edge_tmpl & edge)
        {
            left = edge.left;
            top = edge.top;
            right = edge.right;
            bottom = edge.bottom;
            return *this;
        }

        ValT width() const { return left + right; }
        ValT height() const { return top + bottom; }
        vec2<ValT> size() const { return vec2<ValT>(left + right, top + bottom); }

        edge_tmpl operator +(const edge_tmpl & edge) const
        {
            return edge_tmpl(left + edge.left, top + edge.top,
                right + edge.right, bottom + edge.bottom);
        }

        edge_tmpl & operator +=(const edge_tmpl & edge)
        {
            left += edge.left;
            top += edge.top;
            right += edge.right;
            bottom += edge.bottom;
            return *this;
        }

        edge_tmpl operator -(const edge_tmpl & edge) const
        {
            return edge_tmpl(left - edge.left, top - edge.top,
                right - edge.right, bottom - edge.bottom);
        }

        edge_tmpl & operator -=(const edge_tmpl & edge)
        {
            left -= edge.left;
            top -= edge.top;
            right -= edge.right;
            bottom -= edge.bottom;
            return *this;
        }

        bool operator ==(const edge_tmpl & another) const
        {
            return left == another.left &&
                top == another.top &&
                right == another.right &&
                bottom == another.bottom;
        }

        bool operator !=(const edge_tmpl & another) const
        {
            return !operator ==(another);
        }

        bool is_empty() const { return left <= 0 && top <= 0 && right <= 0 && bottom <= 0; }
    public:
        union
        {
            struct
            {
                vec2<ValT> lefttop;
                vec2<ValT> rightbottom;
            };

            struct
            {
                ValT left;
                ValT top;
                ValT right;
                ValT bottom;
            };
        };
    };
}