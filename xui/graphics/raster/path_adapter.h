#pragma once

namespace agg
{
    template<size_t count>
    class path_adaptor : public graphics::raster::path
    {
    public:
        void push(core::pt64f coord, agg::path_commands_e cmd)
        {
            _data[_index].coord = coord;
            _data[_index].cmd = cmd;
            ++_index;
        }

        void close()
        {
            _data[_index].cmd = path_cmd_end_poly | path_flags_close | path_flags_ccw;
            ++_index;
        }

        void rewind(unsigned)
        {
            status = 0;
        }

        unsigned vertex(double * x, double * y)
        {
            if (status >= count)
                return agg::path_cmd_stop;

            const vertex_t & vt = _data[status++];
            *x = vt.coord.x;
            *y = vt.coord.y;
            return vt.cmd;
        }

    private:
        struct vertex_t
        {
            uint32_t cmd = agg::path_cmd_stop;
            core::pt64f coord;
        };

        uint32_t status = 0;
        std::array<vertex_t, count> _data;
        size_t _index = 0;
    };
}
