#pragma once
#include "stream.h"
#include <fstream>
#include "buffer.h"

namespace core { namespace io
{
    using namespace core;

    class filestream : public core::io::istream
    {
    public:
        filestream();
        filestream(std::string path);
        ~filestream();

        std::tuple<core::error_e, int64_t> read(std::shared_ptr<byte_t> buffer, int64_t nbytes);

    protected:
        std::fstream _fs;
    };

    std::tuple<std::shared_ptr<byte_t>, int64_t> readFullFile(std::string path);
}}
