#pragma once
#include "stream.h"
#include <fstream>

namespace core
{
    namespace io
    {
        using namespace common;

        class filestream : public core::io::istream
        {
        public:
            filestream();
            filestream(std::string path);
            ~filestream();

            std::tuple<common::error_e, int64_t> read(std::shared_ptr<byte_t> buffer, int64_t nbytes);

        protected:
            std::fstream _fs;
        };
    }
}
