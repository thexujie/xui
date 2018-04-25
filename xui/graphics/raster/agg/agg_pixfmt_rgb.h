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
//
// Adaptation for high precision colors has been sponsored by 
// Liberty Technology Systems, Inc., visit http://lib-sys.com
//
// Liberty Technology Systems, Inc. is the provider of
// PostScript and PDF technology for software developers.
// 
//----------------------------------------------------------------------------


#pragma once

#include <string.h>
#include "agg_basics.h"
#include "agg_color_rgba.h"
#include "agg_rendering_buffer.h"

namespace agg
{
    //=====================================================apply_gamma_dir_rgb
    template<class ColorT, class Order, class GammaLut>
    class apply_gamma_dir_rgb
    {
    public:
        typedef typename ColorT::value_type value_type;

        apply_gamma_dir_rgb(const GammaLut & gamma) : m_gamma(gamma) {}

        AGG_INLINE void operator ()(value_type * p)
        {
            p[Order::R] = m_gamma.dir(p[Order::R]);
            p[Order::G] = m_gamma.dir(p[Order::G]);
            p[Order::B] = m_gamma.dir(p[Order::B]);
        }

    private:
        const GammaLut & m_gamma;
    };


    //=====================================================apply_gamma_inv_rgb
    template<class ColorT, class Order, class GammaLut>
    class apply_gamma_inv_rgb
    {
    public:
        typedef typename ColorT::value_type value_type;

        apply_gamma_inv_rgb(const GammaLut & gamma) : m_gamma(gamma) {}

        AGG_INLINE void operator ()(value_type * p)
        {
            p[Order::R] = m_gamma.inv(p[Order::R]);
            p[Order::G] = m_gamma.inv(p[Order::G]);
            p[Order::B] = m_gamma.inv(p[Order::B]);
        }

    private:
        const GammaLut & m_gamma;
    };


    //=========================================================blender_rgb
    template<class ColorT, class Order>
    struct pixfmt_rgb
    {
        typedef ColorT color_type;
        typedef Order order_type;
        typedef typename color_type::value_type value_type;
        typedef typename color_type::calc_type calc_type;

        enum base_scale_e { base_shift = color_type::base_shift };

        //--------------------------------------------------------------------
        static AGG_INLINE void blend_pix(value_type * p,
            unsigned cr, unsigned cg, unsigned cb,
            unsigned alpha,
            unsigned cover = 0)
        {
            p[Order::R] += (value_type)(((cr - p[Order::R]) * alpha) >> base_shift);
            p[Order::G] += (value_type)(((cg - p[Order::G]) * alpha) >> base_shift);
            p[Order::B] += (value_type)(((cb - p[Order::B]) * alpha) >> base_shift);
        }
    };


    //======================================================blender_rgb_pre
    template<class ColorT, class Order>
    struct blender_rgb_pre
    {
        typedef ColorT color_type;
        typedef Order order_type;
        typedef typename color_type::value_type value_type;
        typedef typename color_type::calc_type calc_type;

        enum base_scale_e { base_shift = color_type::base_shift };

        //--------------------------------------------------------------------
        static AGG_INLINE void blend_pix(value_type * p,
            unsigned cr, unsigned cg, unsigned cb,
            unsigned alpha,
            unsigned cover)
        {
            alpha = color_type::base_mask - alpha;
            cover = (cover + 1) << (base_shift - 8);
            p[Order::R] = (value_type)((p[Order::R] * alpha + cr * cover) >> base_shift);
            p[Order::G] = (value_type)((p[Order::G] * alpha + cg * cover) >> base_shift);
            p[Order::B] = (value_type)((p[Order::B] * alpha + cb * cover) >> base_shift);
        }

        //--------------------------------------------------------------------
        static AGG_INLINE void blend_pix(value_type * p,
            unsigned cr, unsigned cg, unsigned cb,
            unsigned alpha)
        {
            alpha = color_type::base_mask - alpha;
            p[Order::R] = (value_type)(((p[Order::R] * alpha) >> base_shift) + cr);
            p[Order::G] = (value_type)(((p[Order::G] * alpha) >> base_shift) + cg);
            p[Order::B] = (value_type)(((p[Order::B] * alpha) >> base_shift) + cb);
        }
    };


    //===================================================blender_rgb_gamma
    template<class ColorT, class Order, class Gamma>
    class blender_rgb_gamma
    {
    public:
        typedef ColorT color_type;
        typedef Order order_type;
        typedef Gamma gamma_type;
        typedef typename color_type::value_type value_type;
        typedef typename color_type::calc_type calc_type;

        enum base_scale_e { base_shift = color_type::base_shift };

        //--------------------------------------------------------------------
        blender_rgb_gamma() : m_gamma(0) {}
        void gamma(const gamma_type & g) { m_gamma = &g; }

        //--------------------------------------------------------------------
        AGG_INLINE void blend_pix(value_type * p,
            unsigned cr, unsigned cg, unsigned cb,
            unsigned alpha,
            unsigned cover = 0)
        {
            calc_type r = m_gamma->dir(p[Order::R]);
            calc_type g = m_gamma->dir(p[Order::G]);
            calc_type b = m_gamma->dir(p[Order::B]);
            p[Order::R] = m_gamma->inv((((m_gamma->dir(cr) - r) * alpha) >> base_shift) + r);
            p[Order::G] = m_gamma->inv((((m_gamma->dir(cg) - g) * alpha) >> base_shift) + g);
            p[Order::B] = m_gamma->inv((((m_gamma->dir(cb) - b) * alpha) >> base_shift) + b);
        }

    private:
        const gamma_type * m_gamma;
    };


    //==================================================pixel_accessor_alpha_blend_rgb
    template<typename PixelT>
    class pixel_accessor_rgb : public pixel_accessor<PixelT>
    {
    public:
        typedef typename PixelT::color_type color_type;
        typedef typename PixelT::order_type order_type;
        typedef typename color_type::value_type value_type;
        typedef typename color_type::calc_type calc_type;

        //typedef typename blender<PixelT> blender_type;
        enum base_scale_e
        {
            base_shift = color_type::base_shift,
            base_scale = color_type::base_scale,
            base_mask = color_type::base_mask,
            pix_width = sizeof(value_type) * 3
        };

    public:
        //--------------------------------------------------------------------
        explicit pixel_accessor_rgb(row_accessor & racc) : m_rbuf(racc) {}

        //void attach(row_accessor & rb) { m_rbuf = rb; }

        //--------------------------------------------------------------------
        template<class PixFmt>
        bool attach(PixFmt & pixf, int x1, int y1, int x2, int y2)
        {
            rect_i r(x1, y1, x2, y2);
            if (r.clip(rect_i(0, 0, pixf.width() - 1, pixf.height() - 1)))
            {
                int stride = pixf.stride();
                m_rbuf.attach(pixf.pix_ptr(r.x1, stride < 0 ? r.y2 : r.y1),
                    (r.x2 - r.x1) + 1,
                    (r.y2 - r.y1) + 1,
                    stride);
                return true;
            }
            return false;
        }

        unsigned width() const { return m_rbuf.width(); }
        unsigned height() const { return m_rbuf.height(); }
        int stride() const { return m_rbuf.stride(); }

        int8u * row_ptr(int y) { return m_rbuf.row_ptr(y); }
        const int8u * row_ptr(int y) const { return m_rbuf.row_ptr(y); }
        const_row_info<int8u> row(int y) const { return m_rbuf.row(y); }

        //--------------------------------------------------------------------
        AGG_INLINE int8u * pix_ptr(int x, int y)
        {
            return m_rbuf.row_ptr(y) + x * pix_width;
        }

        AGG_INLINE const int8u * pix_ptr(int x, int y) const
        {
            return m_rbuf.row_ptr(y) + x * pix_width;
        }

        //--------------------------------------------------------------------
        AGG_INLINE static void make_pix(int8u * p, const color_type & c)
        {
            ((value_type*)p)[order_type::R] = c.r;
            ((value_type*)p)[order_type::G] = c.g;
            ((value_type*)p)[order_type::B] = c.b;
        }

        //--------------------------------------------------------------------
        AGG_INLINE color_type pixel(int x, int y) const
        {
            value_type * p = (value_type*)m_rbuf.row_ptr(y) + x + x + x;
            return color_type(p[order_type::R],
                p[order_type::G],
                p[order_type::B]);
        }

        //--------------------------------------------------------------------
        AGG_INLINE void copy_pixel(int x, int y, const color_type & c)
        {
            value_type * p = (value_type*)m_rbuf.row_ptr(x, y, 1) + x + x + x;
            p[order_type::R] = c.r;
            p[order_type::G] = c.g;
            p[order_type::B] = c.b;
        }

        //--------------------------------------------------------------------
        AGG_INLINE void copy_hline(int x, int y, unsigned len, const color_type & c)
        {
            value_type * p = (value_type*)m_rbuf.row_ptr(x, y, len) + x + x + x;
            do
            {
                p[order_type::R] = c.r;
                p[order_type::G] = c.g;
                p[order_type::B] = c.b;
                p += 3;
            }
            while (--len);
        }

        //--------------------------------------------------------------------
        AGG_INLINE void copy_vline(int x, int y, unsigned len, const color_type & c)
        {
            do
            {
                value_type * p = (value_type*)
                    m_rbuf.row_ptr(x, y++, 1) + x + x + x;
                p[order_type::R] = c.r;
                p[order_type::G] = c.g;
                p[order_type::B] = c.b;
            } while (--len);
        }



        //--------------------------------------------------------------------
        void copy_color_hspan(int x, int y, unsigned len, const color_type * colors)
        {
            value_type * p = (value_type*)
                m_rbuf.row_ptr(x, y, len) + x + x + x;

            do
            {
                p[order_type::R] = colors->r;
                p[order_type::G] = colors->g;
                p[order_type::B] = colors->b;
                ++colors;
                p += 3;
            } while (--len);
        }


        //--------------------------------------------------------------------
        void copy_color_vspan(int x, int y, unsigned len, const color_type * colors)
        {
            do
            {
                value_type * p = (value_type*)
                    m_rbuf.row_ptr(x, y++, 1) + x + x + x;
                p[order_type::R] = colors->r;
                p[order_type::G] = colors->g;
                p[order_type::B] = colors->b;
                ++colors;
            } while (--len);
        }

    private:
        row_accessor & m_rbuf;
    };


    typedef pixfmt_rgb<rgba8, order_rgb> pixfmt_rgb24;
    typedef pixfmt_rgb<rgba8, order_bgr> pixfmt_bgr24;

    typedef pixel_accessor_rgb<pixfmt_rgb<rgba8, order_rgb>> pixel_accessor_rgb24; //----pixel_accessor_rgb24
    typedef pixel_accessor_rgb<pixfmt_rgb<rgba8, order_bgr>> pixel_accessor_bgr24; //----pixel_accessor_bgr24
}
