#pragma once

namespace core::io
{
    struct buffer
    {
        std::shared_ptr<byte_t> data;
        int32_t size;
    };
}
