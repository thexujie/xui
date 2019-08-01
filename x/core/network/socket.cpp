#include "stdafx.h"
#include "socket.h"

#include "win32/win32.h"
#include <WinSock2.h>
#include <WS2tcpip.h>

namespace core::network
{
    using namespace core;

    socket::socket() { }

    socket::socket(const std::u8string & host, port_t port, std::chrono::microseconds timeout)
    {
        connect(host, port, timeout);
    }

    socket::socket(netaddr addr, port_t port, std::chrono::microseconds timeout)
    {
        connect(addr, port, timeout);
    }

    socket::~socket()
    {
        close();
    }

    socket_state socket::state() const
    {
        return _state;
    }

    error socket::set_timeout(std::chrono::microseconds timeout_read, std::chrono::microseconds timeout_write)
    {
        _timeout_read = timeout_read;
        _timeout_send = timeout_write;
        if (_fd != invalid_fd)
        {
            uint32_t to_read = (uint32_t)(_timeout_read.count() / 1000);
            uint32_t to_send = (uint32_t)(_timeout_send.count() / 1000);
            setsockopt(reinterpret_cast<SOCKET>(_fd), SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char *>(&to_read), sizeof(timeval));
            setsockopt(reinterpret_cast<SOCKET>(_fd), SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char *>(&to_send), sizeof(timeval));
        }
        return ok;
    }

    error socket::connect(const std::u8string & host, port_t port, std::chrono::microseconds timeout)
    {
        std::vector<netaddr> addrs = host2addrs(host);
        if (addrs.empty())
            return e_unreachable;
        return connect(addrs[std::random_device()() % addrs.size()], port, timeout);
    }

    error socket::connect(netaddr addr, port_t port, std::chrono::microseconds timeout)
    {
        if (_state != socket_state_closed)
            return e_state;

        if (_fd == invalid_fd)
        {
            _fd = reinterpret_cast<void *>(::socket(AF_INET, _type, 0));
            if (_fd == invalid_fd)
                return e_generic;

            uint32_t to_read = (uint32_t)(_timeout_read.count() / 1000);
            uint32_t to_send = (uint32_t)(_timeout_send.count() / 1000);
            setsockopt(reinterpret_cast<SOCKET>(_fd), SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char *>(&to_read), sizeof(timeval));
            setsockopt(reinterpret_cast<SOCKET>(_fd), SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char *>(&to_send), sizeof(timeval));
        }

        sockaddr_in saddr = {};
        saddr.sin_family = 2;
        saddr.sin_port = ntohs((u_short)port);
        saddr.sin_addr.S_un.S_un_b.s_b1 = addr.v4[0];
        saddr.sin_addr.S_un.S_un_b.s_b2 = addr.v4[1];
        saddr.sin_addr.S_un.S_un_b.s_b3 = addr.v4[2];
        saddr.sin_addr.S_un.S_un_b.s_b4 = addr.v4[3];
        if (::connect(reinterpret_cast<SOCKET>(_fd), (sockaddr *)&saddr, sizeof(sockaddr_in)) == -1)
        {
            uint32_t err = WSAGetLastError();
            return e_generic;
        }
        _state = socket_state_connected;
        return ok;
    }

    void socket::close()
    {
        if (_fd != invalid_fd)
        {
            closesocket(reinterpret_cast<SOCKET>(_fd));
            _fd = invalid_fd;
            _state = socket_state_closed;
        }
    }

    error socket::select(std::chrono::microseconds timeout)
    {
        if (_state != socket_state_connected)
            return e_state;

        fd_set readSets = {};
        fd_set writeSets = {};
        fd_set errorSets = {};
        FD_SET((SOCKET)_fd, &readSets);
        FD_SET((SOCKET)_fd, &errorSets);

        timeval tv = { static_cast<long>(timeout.count() / 1000000), static_cast<long>(timeout.count() % 1000000) };
        int ret = ::select(0, &readSets, &writeSets, &errorSets, &tv);
        if (ret == 0)
            return e_timeout;

        if (ret < 0)
        {
            int wserr = WSAGetLastError();
            return e_generic;
        }

        if (FD_ISSET((SOCKET)_fd, &errorSets))
        {
            assert(false);
            return e_generic;
        }
        return ok;
    }

    std::tuple<error, int64_t> socket::send(std::shared_ptr<byte_t> buffer, int64_t nbytes)
    {
        if (_fd == invalid_fd)
            return { e_state, 0 };

        int ret = ::send(reinterpret_cast<SOCKET>(_fd), buffer.get(), static_cast<int32_t>(nbytes), 0);
        assert(ret >= 0);
        if (ret < 0)
        {
            _ws_error = WSAGetLastError();
            logger::err() << __FUNCTION__" send 0x" << std::hex << buffer.get() << " " << nbytes << " bytes, %d" << win32::winerr_str(_ws_error);
            return { e_io, 0 };
        }
        else
            return { ok, ret };
    }

    std::tuple<error, int64_t> socket::send(std::vector<byte_t> & buffer, int64_t nbytes)
    {
        return recieve(std::shared_ptr<byte_t>(buffer.data(), [](void *) {}), nbytes);
    }

    std::tuple<error, int64_t> socket::recieve(std::shared_ptr<byte_t> buffer, int64_t nbytes)
    {
        if (_fd == invalid_fd || _state != socket_state_connected)
            return { e_state, 0 };

        int32_t ret = recv((SOCKET)_fd, buffer.get(), static_cast<int32_t>(nbytes), 0);
        assert(ret >= 0);
        if (ret < 0)
        {
            _ws_error = WSAGetLastError();
            logger::err() << __FUNCTION__" recv 0x" << std::hex << buffer.get() << " " << nbytes << " bytes, error %d" << win32::winerr_str(_ws_error);
            return { e_io, 0 };
        }
        else if (ret == 0)
            return { e_broken, 0 };
        else
            return { ok, ret };
    }

    std::tuple<error, int64_t> socket::recieve(std::vector<byte_t> & buffer, int64_t nbytes)
    {
        return recieve(std::shared_ptr<byte_t>(buffer.data(), [](void *) {}), nbytes);
    }
}
