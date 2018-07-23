#pragma once

#include "addr.h"

namespace core::network
{
    const auto invalid_fd = reinterpret_cast<void *>(~0);

    enum socket_type
    {
        socket_type_steam = 1,
        socket_type_dgram,
    };

    enum socket_state
    {
        socket_state_closed = 0,
        socket_state_connecting,
        socket_state_connected,
        socket_state_closing,
    };

    class socket
    {
    public:
        socket();
        socket(std::string host, port_t port, std::chrono::microseconds timeout);
        socket(netaddr addr, port_t port, std::chrono::microseconds timeout);
        ~socket();

        socket_state state() const;
        core::error set_timeout(std::chrono::microseconds timeout_read, std::chrono::microseconds timeout_write);
        core::error connect(std::string host, port_t port, std::chrono::microseconds timeout);
        core::error connect(netaddr addr, port_t port, std::chrono::microseconds timeout);
        void close();

        core::error select(std::chrono::microseconds timeout);

        std::tuple<core::error, int64_t> send(std::shared_ptr<byte_t> buffer, int64_t nbytes);
        std::tuple<core::error, int64_t> send(std::vector<byte_t> & buffer, int64_t nbyte);

        std::tuple<core::error, int64_t> recieve(std::shared_ptr<byte_t> buffer, int64_t nbytes);
        std::tuple<core::error, int64_t> recieve(std::vector<byte_t> & buffer, int64_t nbyte);

    protected:
        void * _fd = invalid_fd;
        socket_type _type = socket_type_steam;
        socket_state _state = socket_state_closed;
        std::chrono::microseconds _timeout_read = 6s;
        std::chrono::microseconds _timeout_send = 6s;
        int32_t _ws_error = 0;
    };
}
