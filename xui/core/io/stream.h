#pragma once

#include "common/error.h"

namespace core
{
    namespace io
    {
        inline std::shared_ptr<byte_t> grab(std::vector<byte_t> & buffer)
        {
            return std::shared_ptr<byte_t>(buffer.data(), [](byte_t *){});
        }

        inline std::shared_ptr<byte_t> grab(byte_t * buffer)
        {
            return std::shared_ptr<byte_t>(static_cast<byte_t *>(buffer), [](byte_t *) {});
        }

        template<int SIZE>
        inline std::shared_ptr<byte_t> grab(std::array<byte_t, SIZE> & buffer)
        {
            return std::shared_ptr<byte_t>(buffer.data(), [](byte_t *) {});
        }

        //inline std::tuple<std::shared_ptr<byte_t>, int64_t> grab(std::vector<byte_t> & buffer)
        //{
        //    return { std::shared_ptr<byte_t>(buffer.data(), [](byte_t *) {}), buffer.size() };
        //}

        struct rawstreambuf : std::streambuf
        {
            rawstreambuf(char const * data, size_t size)
            {
                setg(const_cast<char *>(data), const_cast<char *>(data), const_cast<char *>(data + size));
            }
        };

        class istream
        {
        public:
            virtual ~istream() = default;

            virtual std::tuple<common::error_e, int64_t> read(std::shared_ptr<byte_t> buffer, int64_t nbytes) = 0;
        };
    }
}

