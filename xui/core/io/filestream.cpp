#include "stdafx.h"
#include "filestream.h"

namespace core
{
    namespace io
    {
        filestream::filestream()
        {
            
        }

        filestream::filestream(std::string path)
        {
            _fs.open(path, std::ios_base::binary | std::ios_base::in);
        }

        filestream::~filestream()
        {

        }

        std::tuple<common::error_e, int64_t> filestream::read(std::shared_ptr<byte_t> buffer, int64_t nbytes)
        {
            if (!_fs.is_open())
                return { error_io, 0 };

            if (_fs.eof())
                return { error_eof, 0 };

            if(!_fs.good())
                return { error_io, 0 };

            _fs.read(buffer.get(), static_cast<std::streamsize>(nbytes));
            return { error_ok, _fs.gcount() };
        }
    }
}