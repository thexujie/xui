#pragma once

namespace agg
{
    //=====================================span_image_filter_rgb_bilinear_clip
    template<class SrcPixelT, class DstPixelT>
    class span_image_filter_rgb_bilinear_clip : public span_colorer<DstPixelT>
    {
    public:
        typedef pixel_accessor<SrcPixelT> source_type;
        typedef typename SrcPixelT::order_type order_type;
        typedef typename SrcPixelT::color_type color_type;
        typedef typename SrcPixelT::color_type::value_type value_type;
        typedef typename SrcPixelT::color_type::calc_type calc_type;

        enum base_scale_e
        {
            base_shift = color_type::base_shift,
            base_mask = color_type::base_mask
        };

        span_image_filter_rgb_bilinear_clip(source_type & src, const agg::trans_affine & trans, const color_type & back_color) :
            m_source(src), m_trans(trans), m_back_color(back_color),
            m_dx_dbl(0.5),
            m_dy_dbl(0.5),
            m_dx_int(image_subpixel_scale / 2),
            m_dy_int(image_subpixel_scale / 2) {}

        const color_type & background_color() const { return m_back_color; }
        void background_color(const color_type & v) { m_back_color = v; }

        int filter_dx_int() const { return m_dx_int; }
        int filter_dy_int() const { return m_dy_int; }
        double filter_dx_dbl() const { return m_dx_dbl; }
        double filter_dy_dbl() const { return m_dy_dbl; }

        //--------------------------------------------------------------------
        void prepare() {}
        void generate(DstPixelT * span, int x, int y, unsigned len)
        {
            agg::span_interpolator_linear<> interpolator(m_trans);
            interpolator.begin(x + filter_dx_dbl(), y + filter_dy_dbl(), len);

            calc_type colors[3];
            calc_type src_alpha;
            value_type back_r = m_back_color.r;
            value_type back_g = m_back_color.g;
            value_type back_b = m_back_color.b;
            value_type back_a = m_back_color.a;

            const value_type * fg_ptr;

            int maxx = m_source.width() - 1;
            int maxy = m_source.height() - 1;

            do
            {
                int x_hr;
                int y_hr;

                interpolator.coordinates(&x_hr, &y_hr);

                x_hr -= filter_dx_int();
                y_hr -= filter_dy_int();

                int x_lr = x_hr >> image_subpixel_shift;
                int y_lr = y_hr >> image_subpixel_shift;
                unsigned weight;

                if (x_lr >= 0 && y_lr >= 0 && x_lr < maxx && y_lr < maxy)
                {
                    colors[0] = colors[1] = colors[2] = image_subpixel_scale * image_subpixel_scale / 2;

                    x_hr &= image_subpixel_mask;
                    y_hr &= image_subpixel_mask;

                    fg_ptr = (const value_type *)m_source.row_ptr(y_lr) + x_lr + x_lr + x_lr;

                    // left top
                    weight = (image_subpixel_scale - x_hr) * (image_subpixel_scale - y_hr);
                    colors[0] += weight * *fg_ptr++;
                    colors[1] += weight * *fg_ptr++;
                    colors[2] += weight * *fg_ptr++;

                    // right top
                    weight = x_hr * (image_subpixel_scale - y_hr);
                    colors[0] += weight * *fg_ptr++;
                    colors[1] += weight * *fg_ptr++;
                    colors[2] += weight * *fg_ptr++;

                    ++y_lr;
                    fg_ptr = (const value_type*)m_source.row_ptr(y_lr) + x_lr + x_lr + x_lr;

                    // left bottom
                    weight = (image_subpixel_scale - x_hr) * y_hr;
                    colors[0] += weight * *fg_ptr++;
                    colors[1] += weight * *fg_ptr++;
                    colors[2] += weight * *fg_ptr++;

                    // right bottom
                    weight = x_hr * y_hr;
                    colors[0] += weight * *fg_ptr++;
                    colors[1] += weight * *fg_ptr++;
                    colors[2] += weight * *fg_ptr++;

                    colors[0] >>= image_subpixel_shift * 2;
                    colors[1] >>= image_subpixel_shift * 2;
                    colors[2] >>= image_subpixel_shift * 2;
                    src_alpha = base_mask;
                }
                else
                {
                    if (x_lr < -1 || y_lr < -1 || x_lr > maxx || y_lr > maxy)
                    {
                        colors[order_type::R] = back_r;
                        colors[order_type::G] = back_g;
                        colors[order_type::B] = back_b;
                        src_alpha = back_a;
                    }
                    else
                    {
                        colors[0] = colors[1] = colors[2] = src_alpha = image_subpixel_scale * image_subpixel_scale / 2;

                        x_hr &= image_subpixel_mask;
                        y_hr &= image_subpixel_mask;

                        weight = (image_subpixel_scale - x_hr) * (image_subpixel_scale - y_hr);
                        if (x_lr >= 0 && y_lr >= 0 && x_lr <= maxx && y_lr <= maxy)
                        {
                            fg_ptr = (const value_type*)m_source.row_ptr(y_lr) + x_lr + x_lr + x_lr;

                            colors[0] += weight * *fg_ptr++;
                            colors[1] += weight * *fg_ptr++;
                            colors[2] += weight * *fg_ptr++;
                            src_alpha += weight * base_mask;
                        }
                        else
                        {
                            colors[order_type::R] += back_r * weight;
                            colors[order_type::G] += back_g * weight;
                            colors[order_type::B] += back_b * weight;
                            src_alpha += back_a * weight;
                        }

                        x_lr++;

                        weight = x_hr * (image_subpixel_scale - y_hr);
                        if (x_lr >= 0 && y_lr >= 0 &&
                            x_lr <= maxx && y_lr <= maxy)
                        {
                            fg_ptr = (const value_type*)m_source.row_ptr(y_lr) + x_lr + x_lr + x_lr;

                            colors[0] += weight * *fg_ptr++;
                            colors[1] += weight * *fg_ptr++;
                            colors[2] += weight * *fg_ptr++;
                            src_alpha += weight * base_mask;
                        }
                        else
                        {
                            colors[order_type::R] += back_r * weight;
                            colors[order_type::G] += back_g * weight;
                            colors[order_type::B] += back_b * weight;
                            src_alpha += back_a * weight;
                        }

                        x_lr--;
                        y_lr++;

                        weight = (image_subpixel_scale - x_hr) * y_hr;
                        if (x_lr >= 0 && y_lr >= 0 &&
                            x_lr <= maxx && y_lr <= maxy)
                        {
                            fg_ptr = (const value_type*)m_source.row_ptr(y_lr) + x_lr + x_lr + x_lr;

                            colors[0] += weight * *fg_ptr++;
                            colors[1] += weight * *fg_ptr++;
                            colors[2] += weight * *fg_ptr++;
                            src_alpha += weight * base_mask;
                        }
                        else
                        {
                            colors[order_type::R] += back_r * weight;
                            colors[order_type::G] += back_g * weight;
                            colors[order_type::B] += back_b * weight;
                            src_alpha += back_a * weight;
                        }

                        x_lr++;

                        weight = x_hr * y_hr;
                        if (x_lr >= 0 && y_lr >= 0 && x_lr <= maxx && y_lr <= maxy)
                        {
                            fg_ptr = (const value_type*)m_source.row_ptr(y_lr) + x_lr + x_lr + x_lr;

                            colors[0] += weight * *fg_ptr++;
                            colors[1] += weight * *fg_ptr++;
                            colors[2] += weight * *fg_ptr++;
                            src_alpha += weight * base_mask;
                        }
                        else
                        {
                            colors[order_type::R] += back_r * weight;
                            colors[order_type::G] += back_g * weight;
                            colors[order_type::B] += back_b * weight;
                            src_alpha += back_a * weight;
                        }

                        colors[0] >>= image_subpixel_shift * 2;
                        colors[1] >>= image_subpixel_shift * 2;
                        colors[2] >>= image_subpixel_shift * 2;
                        src_alpha >>= image_subpixel_shift * 2;
                    }
                }

                span->r = (value_type)colors[order_type::R];
                span->g = (value_type)colors[order_type::G];
                span->b = (value_type)colors[order_type::B];
                span->a = (value_type)src_alpha;

                ++span;
                ++interpolator;
            } while (--len);
        }

    private:
        source_type & m_source;
        agg::trans_affine m_trans;
        color_type m_back_color;

        double m_dx_dbl;
        double m_dy_dbl;
        unsigned m_dx_int;
        unsigned m_dy_int;
    };


}
