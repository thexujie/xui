#pragma once
#include "stream.h"
#include <fstream>
#include "buffer.h"

namespace core::io
{
    using namespace core;
    using namespace std::experimental;

    class filestream : public core::io::istream
    {
    public:
        filestream();
        filestream(std::string path);
        ~filestream();

        std::tuple<core::error, int64_t> read(std::shared_ptr<byte_t> buffer, int64_t nbytes);

    protected:
        std::fstream _fs;
    };

    std::tuple<std::shared_ptr<byte_t>, int64_t> readFullFile(std::string path);
    std::vector<filesystem::path> allPaths(filesystem::path dirctory);
}
