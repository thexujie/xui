#include "stdafx.h"
#include "http.h"
#include "../string.h"

namespace core::network
{
    using namespace core;

    u8string http_header::string()
    {
        std::ostringstream stream;
        stream << method << " " << url << " " << version << "\r\n";
        for (auto & pair : headers)
            stream << pair.first << ": " << pair.second << "\r\n";
        stream << "\r\n";
        return stream.str();
    }

    http::http() { }

    http::http(netaddr addr, port_t port, std::chrono::microseconds timeout)
    {
        connect(addr, port, timeout);
    }

    http::http(std::string host, port_t port, std::chrono::microseconds timeout)
    {
        connect(host, port, timeout);
    }

    http::http(std::shared_ptr<network::socket> socket) : _socket(socket) { }

    http::~http()
    {
        _socket.reset();
    }

    socket_state http::state() const
    {
        if (!_socket)
            return socket_state_closed;
        return _socket->state();
    }

    std::shared_ptr<socket> http::socket() const
    {
        return _socket;
    }

    core::error_e http::set_timeout(std::chrono::microseconds timeout_read, std::chrono::microseconds timeout_write)
    {
        if (!_socket)
            _socket = std::make_shared<network::socket>();
        return _socket->set_timeout(timeout_read, timeout_write);
    }

    core::error_e http::connect(std::string host, port_t port, std::chrono::microseconds timeout)
    {
        if (_socket)
        {
            if (_socket->state() == socket_state_connected)
                _socket->close();

            if (_socket->state() != socket_state_closed)
                return error_state;
        }
        else
            _socket = std::make_shared<network::socket>();

        error_e error = _socket->connect(host, port, timeout);
        return error;
    }

    core::error_e http::connect(netaddr addr, port_t port, std::chrono::microseconds timeout)
    {
        if (_socket)
        {
            if (_socket->state() == socket_state_connected)
                _socket->close();

            if (_socket->state() != socket_state_closed)
                return error_state;
        }
        else
            _socket = std::make_shared<network::socket>();

        error_e error = _socket->connect(addr, port, timeout);
        return error;
    }

    void http::close()
    {
        if (!_socket)
            return;

        if (_socket->state() == socket_state_connected)
            return;

        _socket->close();
    }

    std::tuple<error_e, int64_t> http::send(const u8string & text)
    {
        if (!_socket || _socket->state() != socket_state_connected)
            return { error_state, 0 };

        auto [error, nb_send] = _socket->send(std::shared_ptr<byte_t>((byte_t *)text.data(), [](byte_t *) {}), text.length());
        return { error, nb_send };
    }

    std::tuple<core::error_e, int64_t> http::read(std::shared_ptr<byte_t> buffer, int64_t nbytes)
    {
        assert(_socket && _socket->state() == socket_state_connected);
        if (!_socket || _socket->state() != socket_state_connected)
            return { error_state, 0 };

        int64_t nb_write = 0;
        if (_buffer_size > _buffer_pos)
        {
            nb_write = std::min(_buffer_size - _buffer_pos, nbytes);
            std::memcpy(buffer.get(), _buffer.get() + _buffer_pos, (size_t)nb_write);
            _buffer_pos += nb_write;

            if (nb_write >= nbytes)
            {
                if (_buffer_pos >= _buffer_size)
                {
                    _buffer.reset();
                    _buffer_size = 0;
                    _buffer_pos = 0;
                }
                return { error_ok, nb_write };
            }

            _nb_recieved += nb_write;
            if (_response->content_length >= 0 && _nb_recieved >= _response->content_length)
            {
                assert((_buffer_size == _buffer_pos));
                if (_buffer_size <= _buffer_pos)
                {
                    _buffer.reset();
                    _buffer_size = 0;
                    _buffer_pos = 0;
                }
                return { error_ok, nb_write };
            }
        }
        else
        {
            if (_buffer_size)
            {
                _buffer.reset();
                _buffer_size = 0;
                _buffer_pos = 0;
            }
        }

        if (_response->content_length >= 0 && _nb_recieved >= _response->content_length)
            return { error_eof, 0 };

        //error_e err_select = _socket->select(50ms);
        //if (err_select < 0)
        //    return err_select;

        auto [err_recv, nb_recv] = _socket->recieve(std::shared_ptr<byte_t>(buffer.get() + nb_write, [](void *) {}), nbytes - nb_write);
        if (err_recv < 0)
            return { err_recv, 0 };

        _nb_recieved += nb_recv;
        nb_write += nb_recv;

        return { error_ok, nb_write };
    }

    std::tuple<core::error_e, std::shared_ptr<http_response>> http::recieve()
    {
        std::shared_ptr<byte_t> buffer(new byte_t[http_patch_size], std::default_delete<byte_t[]>());
        auto [error, response, nb_read] = recieve(buffer, http_patch_size);
        if (error < 0)
            return { error, response };
        _buffer_size = nb_read;
        _buffer_pos = 0;
        _buffer = buffer;

        return { error, response };
    }

    std::tuple<core::error_e, std::shared_ptr<http_response>, int64_t> http::recieve(std::shared_ptr<byte_t> buffer, int64_t nbytes)
    {
        if (!_socket || _socket->state() != socket_state_connected)
            return { error_state, nullptr, 0 };

        _response = std::make_shared<http_response>();
        std::shared_ptr<http_response> response = _response;
        std::vector<byte_t> temp_last;
        std::shared_ptr<byte_t> temp(new byte_t[http_patch_size], std::default_delete<byte_t[]>());
        int iline = 0;
        bool reading = true;
        int64_t nb_read = 0;
        while (reading)
        {
            auto [err_read, nb_read_from_socket] = read(temp, http_patch_size);
            if (err_read < 0)
                return { err_read, response, 0 };
            std::string_view sv(temp.get(), (size_t)nb_read_from_socket);
            int64_t pos = 0;
            while (pos < nb_read_from_socket)
            {
                int64_t pos_rn = sv.find_first_of("\r\n", (size_t)pos, 2);
                if (pos_rn == std::string_view::npos)
                {
                    core::logger::err() << __FUNCTION__" sv.find_first_of npos";
                    printf("error !!!");
                    break;
                }

                // http 头读取结束了
                if (pos_rn == pos)
                {
                    pos += 2;
                    parse_response(*response);
                    int64_t nb_frac = nb_read_from_socket - pos;
                    int64_t nb_need = std::min(nb_frac, nbytes);
                    std::memcpy(buffer.get(), temp.get() + pos, (size_t)nb_need);
                    nb_read = nb_need;
                    reading = false;

                    _buffer = temp;
                    _buffer_size = nb_read_from_socket;
                    _buffer_pos = nb_read;
                    break;
                }

                std::string line;
                if (temp_last.size() > 0)
                {
                    line.append(temp_last.begin(), temp_last.end());
                    temp_last.clear();
                }

                line.append(temp.get() + pos, temp.get() + pos_rn);
                if (!iline)
                {
                    parse_header(line, *response);
                    ++iline;
                }
                else
                    parse_line(line, *response);

                pos = pos_rn + 2;
            }
        }

        return { error_ok, response, nb_read };
    }

    std::tuple<core::error_e, std::shared_ptr<http_response>> http::recieve(std::function<void(std::shared_ptr<byte_t>, int64_t)> callback)
    {
        std::vector<byte_t> buffer(http_patch_size);
        std::tuple<core::error_e, std::shared_ptr<http_response>, int64_t> result = recieve(core::io::grab(buffer), buffer.capacity());
        if (std::get<0>(result) < 0)
            return { std::get<0>(result), std::get<1>(result) };

        while (std::get<0>(result) >= 0)
        {
            if (buffer.size() > 0)
                callback(core::io::grab(buffer), std::get<2>(result));
            result = recieve(core::io::grab(buffer), buffer.capacity());
        }
        return { std::get<0>(result), std::get<1>(result) };
    }

    std::tuple<core::error_e, std::shared_ptr<http_response>> http::request(const u8string & text)
    {
        auto [error, nb_send] = send(text);
        if (error < 0)
            return { error, nullptr };

        return recieve();
    }

    std::tuple<core::error_e, std::shared_ptr<http_response>, int64_t> http::request(const u8string & text, std::shared_ptr<byte_t> buffer, int64_t nbytes)
    {
        auto [error, nb_send] = send(text);
        if (error < 0)
            return { error, nullptr, 0 };

        return recieve(buffer, nbytes);
    }

    std::tuple<core::error_e, std::shared_ptr<http_response>> http::request(const u8string & text, std::function<void(std::shared_ptr<byte_t>, int64_t)> callback)
    {
        auto [error, nb_send] = send(text);
        if (error < 0)
            return { error, nullptr };

        return recieve(callback);
    }

    core::error_e http::parse_header(const std::string line, http_response & response)
    {
        std::vector<std::string> parts = string::split(line, ' ');
        if (parts.size() != 3)
            return error_generic;

        response.version = string::trim(parts[0]);
        response.code = std::stoi(std::string(parts[1]));
        response.message = string::trim(parts[2]);
        return error_ok;
    }

    core::error_e http::parse_line(const std::string line, http_response & response)
    {
        size_t pos_colon = line.find_first_of(": ");
        if (pos_colon == std::string_view::npos)
            return error_generic;

        std::string key = line.substr(0, pos_colon);
        std::string val = line.substr(pos_colon + 2, line.length() - pos_colon - 2);
        response.headers[key] = val;
        return error_ok;
    }

    core::error_e http::parse_response(http_response & response)
    {
        auto iter_keep_alive = response.headers.find("keep-alive");
        if (iter_keep_alive != response.headers.end())
        {
            if (!core::string::equal_ic()(iter_keep_alive->second, "close"))
                response.flags |= http_flag_keep_alive;
        }
        else
        {
            auto iter_connection = response.headers.find("connection");
            if (iter_connection != response.headers.end())
            {
                if (!core::string::equal_ic()(iter_connection->second, "close"))
                    response.flags |= http_flag_keep_alive;
            }
        }

        auto iter_content_length = response.headers.find("content-length");
        if (iter_content_length != response.headers.end())
            response.content_length = std::stoll(iter_content_length->second);
        return error_ok;
    }
}
