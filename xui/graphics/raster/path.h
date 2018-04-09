#pragma once

namespace graphics
{
    namespace raster
    {
        enum command
        {
            none = 0,
            move,
            line,
            close,
        };

        class path
        {
        public:
            virtual ~path() {};
            virtual bool vertex(int32_t & status, float64_t & x, float64_t & y, command & cmd) = 0;
        };
    }
}