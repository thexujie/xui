#pragma once

namespace graphics::raster
{
    class path
    {
    public:
        virtual ~path() {}

        virtual void rewind(uint32_t path_id) = 0;
        virtual uint32_t vertex(float64_t * x, float64_t * y) = 0;
    };
}
