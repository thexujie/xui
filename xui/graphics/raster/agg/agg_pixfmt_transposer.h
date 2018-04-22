//----------------------------------------------------------------------------
// Anti-Grain Geometry (AGG) - Version 2.5
// A high quality rendering engine for C++
// Copyright (C) 2002-2006 Maxim Shemanarev
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://antigrain.com
// 
// AGG is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// AGG is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with AGG; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA 02110-1301, USA.
//----------------------------------------------------------------------------


#pragma once

#include "agg_basics.h"

namespace agg
{
    //=======================================================pixel_accessor_transposer
    template<class PixFmt>
    class pixel_accessor_transposer
    {
    public:
        typedef PixFmt pixel_accessor_type;
        typedef typename pixel_accessor_type::color_type color_type;
        typedef typename pixel_accessor_type::row_data row_data;
        typedef typename color_type::value_type value_type;
        typedef typename color_type::calc_type calc_type;

        //--------------------------------------------------------------------
        pixel_accessor_transposer() : m_pixf(0) {}
        explicit pixel_accessor_transposer(pixel_accessor_type & pixf) : m_pixf(&pixf) {}
        void attach(pixel_accessor_type & pixf) { m_pixf = &pixf; }

        //--------------------------------------------------------------------
        AGG_INLINE unsigned width() const { return m_pixf->height(); }
        AGG_INLINE unsigned height() const { return m_pixf->width(); }

        //--------------------------------------------------------------------
        AGG_INLINE color_type pixel(int x, int y) const
        {
            return m_pixf->pixel(y, x);
        }

        //--------------------------------------------------------------------
        AGG_INLINE void copy_pixel(int x, int y, const color_type & c)
        {
            m_pixf->copy_pixel(y, x, c);
        }

        //--------------------------------------------------------------------
        AGG_INLINE void blend_pixel(int x, int y,
            const color_type & c,
            int8u cover)
        {
            m_pixf->blend_pixel(y, x, c, cover);
        }

        //--------------------------------------------------------------------
        AGG_INLINE void copy_hline(int x, int y,
            unsigned len,
            const color_type & c)
        {
            m_pixf->copy_vline(y, x, len, c);
        }

        //--------------------------------------------------------------------
        AGG_INLINE void copy_vline(int x, int y,
            unsigned len,
            const color_type & c)
        {
            m_pixf->copy_hline(y, x, len, c);
        }

        //--------------------------------------------------------------------
        AGG_INLINE void blend_hline(int x, int y,
            unsigned len,
            const color_type & c,
            int8u cover)
        {
            m_pixf->blend_vline(y, x, len, c, cover);
        }

        //--------------------------------------------------------------------
        AGG_INLINE void blend_vline(int x, int y,
            unsigned len,
            const color_type & c,
            int8u cover)
        {
            m_pixf->blend_hline(y, x, len, c, cover);
        }

        //--------------------------------------------------------------------
        AGG_INLINE void blend_solid_hspan(int x, int y,
            unsigned len,
            const color_type & c,
            const int8u * covers)
        {
            m_pixf->blend_solid_vspan(y, x, len, c, covers);
        }

        //--------------------------------------------------------------------
        AGG_INLINE void blend_solid_vspan(int x, int y,
            unsigned len,
            const color_type & c,
            const int8u * covers)
        {
            m_pixf->blend_solid_hspan(y, x, len, c, covers);
        }

        //--------------------------------------------------------------------
        AGG_INLINE void copy_color_hspan(int x, int y,
            unsigned len,
            const color_type * colors)
        {
            m_pixf->copy_color_vspan(y, x, len, colors);
        }

        //--------------------------------------------------------------------
        AGG_INLINE void copy_color_vspan(int x, int y,
            unsigned len,
            const color_type * colors)
        {
            m_pixf->copy_color_hspan(y, x, len, colors);
        }

        //--------------------------------------------------------------------
        AGG_INLINE void blend_color_hspan(int x, int y,
            unsigned len,
            const color_type * colors,
            const int8u * covers,
            int8u cover)
        {
            m_pixf->blend_color_vspan(y, x, len, colors, covers, cover);
        }

        //--------------------------------------------------------------------
        AGG_INLINE void blend_color_vspan(int x, int y,
            unsigned len,
            const color_type * colors,
            const int8u * covers,
            int8u cover)
        {
            m_pixf->blend_color_hspan(y, x, len, colors, covers, cover);
        }

    private:
        pixel_accessor_type * m_pixf;
    };
}
