#include "stdafx.h"
#include "filestream.h"

namespace core::io
{
    filestream::filestream() { }

    filestream::filestream(const std::u8string & path)
    {
        _fs.open(reinterpret_cast<const std::string &>(path), std::ios_base::binary | std::ios_base::in);
    }

    filestream::~filestream() { }

    std::tuple<core::error, int64_t> filestream::read(std::shared_ptr<byte_t> buffer, int64_t nbytes)
    {
        if (!_fs.is_open())
            return { e_io, 0 };

        if (_fs.eof())
            return { e_eof, 0 };

        if (!_fs.good())
            return { e_io, 0 };

        _fs.read(buffer.get(), static_cast<std::streamsize>(nbytes));
        return { ok, _fs.gcount() };
    }

    std::tuple<std::shared_ptr<byte_t>, int64_t> readFullFile(const std::u8string & path)
    {
        std::fstream fs;
        fs.open(core::u8str_wstr(path), std::ios::in | std::ios::binary);
        if (!fs.good())
            return {};

        fs.seekg(0, std::ios::end);
        auto size = fs.tellg();
        fs.seekg(0, std::ios::beg);

        std::shared_ptr<byte_t> data(new byte_t[size], std::default_delete<byte_t[]>());
        fs.read(data.get(), size);
        if (fs.tellg() != size)
            return {};
        return { data, (int64_t)size };
    }

    std::vector<filesystem::path> allPaths(filesystem::path dirctory)
    {
        std::vector<filesystem::path> paths;
        for (auto & path : filesystem::directory_iterator(dirctory))
            paths.push_back(path);
        return paths;
    }
}
