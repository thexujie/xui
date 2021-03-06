#pragma once

#include "socket.h"
#include "core/string.h"
#include "core/io/stream.h"

namespace core::network
{
    constexpr int64_t http_patch_size = 8 * 1024;

    enum http_flag
    {
        http_flag_keep_alive = 0x0001,
    };

    struct http_header
    {
    public:
        std::string version;
        std::string method;
        std::string url;
        std::map<std::string, std::string> headers;

        std::string string();
    };

    struct http_response
    {
        std::string version;
        int32_t code = 0;
        std::string message;
        std::map<std::string, std::string, core::less_ic> headers;

        uint32_t flags = 0;
        int64_t content_length = -1;
    };

    class http : public io::istream, public std::enable_shared_from_this<http>
    {
    public:
        http();
        http(netaddr addr, port_t port, std::chrono::microseconds timeout);
        http(const std::u8string & host, port_t port, std::chrono::microseconds timeout);
        http(std::shared_ptr<socket> socket);
        ~http();

        socket_state state() const;
        std::shared_ptr<socket> socket() const;

        core::error set_timeout(std::chrono::microseconds timeout_read, std::chrono::microseconds timeout_write);
        core::error connect(const std::u8string & host, port_t port, std::chrono::microseconds timeout);
        core::error connect(netaddr addr, port_t port, std::chrono::microseconds timeout);
        void close();

        std::tuple<core::error, int64_t> send(const std::string & text);
        std::tuple<core::error, int64_t> read(std::shared_ptr<byte_t> buffer, int64_t nbytes);

        //数据较少，可以一次性接收
        std::tuple<core::error, std::shared_ptr<http_response>> recieve();
        std::tuple<core::error, std::shared_ptr<http_response>, int64_t> recieve(std::shared_ptr<byte_t> buffer, int64_t nbytes);
        std::tuple<core::error, std::shared_ptr<http_response>> recieve(std::function<void(std::shared_ptr<byte_t>, int64_t)> callback);

        std::tuple<core::error, std::shared_ptr<http_response>> request(const std::string & text);
        std::tuple<core::error, std::shared_ptr<http_response>, int64_t> request(const std::string & text, std::shared_ptr<byte_t> buffer, int64_t nbytes);
        std::tuple<core::error, std::shared_ptr<http_response>> request(const std::string & text, std::function<void(std::shared_ptr<byte_t>, int64_t)> callback);

    private:
        core::error parse_header(const std::string line, http_response & response);
        core::error parse_line(const std::string line, http_response & response);
        core::error parse_response(http_response & response);

    protected:
        std::shared_ptr<network::socket> _socket;

        std::shared_ptr<http_response> _response;
        int64_t _nb_recieved = 0;
        int64_t _timeout_max = 6;

        std::shared_ptr<byte_t> _buffer;
        int64_t _buffer_size = 0;
        int64_t _buffer_pos = 0;
    };
}
