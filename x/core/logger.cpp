#include "stdafx.h"
#include "logger.h"
#include <sys/timeb.h>

namespace core
{
    logger_stream::logger_stream(class logger & logger, log_e lg) : std::ostream(this), _lg_curr(lg), _logger(logger)
    {
        if (&_logger != nullptr)
            _lg = _logger.lg();
    }

    logger_stream::logger_stream(const logger_stream & another) : std::ostream(this), _lg(another._lg), _lg_curr(another._lg_curr), _logger(another._logger) {}

    logger_stream::~logger_stream()
    {
        if (_buffer.in_avail())
        {
            if (&_logger != nullptr)
            {
                _logger.line(_lg_curr, _buffer.str());
                _logger.flush();
            }
            _buffer.pubseekoff(0, std::ios::beg);
        }
    }

    int logger_stream::overflow(int ch)
    {
        if (_lg_curr < _lg)
            return 0;

        return &_logger != nullptr ? _buffer.sputc(ch) : 0;
    }

    std::streamsize logger_stream::xsputn(const char * s, std::streamsize n)
    {
        if (_lg_curr < _lg)
            return 0;

        return &_logger != nullptr ? _buffer.sputn(s, n) : 0;
    }

    int logger_stream::logger_stream::sync()
    {
        if (_lg_curr < _lg)
            return 0;

        if (&_logger != nullptr)
            _logger.flush();
        return 0;
    }

    logger::logger() { }

    logger::logger(std::string path, log_e lg): _lg(lg)
    {
        open(path);
    }

    logger::~logger()
    {
        std::stringstream fs;
    }

    error logger::open(std::string path)
    {
        if (path.empty())
            return error_args;

        std::lock_guard<std::mutex> lock(_mtx);
        _fs.open(path, std::fstream::binary | std::fstream::out | std::fstream::app);
        return _fs.good() ? error_ok : error_io;
    }

    void logger::close()
    {
        std::lock_guard<std::mutex> lock(_mtx);
        if (_fs.good())
        {
            _fs.flush();
            _fs.close();
        }
    }

    void logger::flush()
    {
        if (_fs.good())
            _fs.flush();
    }

    error logger::write(log_e lg, std::string text)
    {
        if (!_fs.good())
            return error_state;

        std::lock_guard<std::mutex> lock(_mtx);
        _write_time(lg);
        _fs << text;
        return _fs.good() ? error_ok : error_io;
    }

    error logger::write(log_e lg, std::string_view text)
    {
        if (!_fs.good())
            return error_state;

        std::lock_guard<std::mutex> lock(_mtx);
        _write_time(lg);
        _fs.write(text.data(), text.length());
        return _fs.good() ? error_ok : error_io;
    }

    error logger::line(log_e lg)
    {
        if (!_fs.good())
            return error_state;

        if (lg < _lg)
            return error_ok;

        std::lock_guard<std::mutex> lock(_mtx);
        _write_time(lg);
        ++_line;
        return _fs.good() ? error_ok : error_io;
    }

    error logger::line(log_e lg, std::string text)
    {
        if (!_fs.good())
            return error_state;

        if (lg < _lg)
            return error_ok;

        std::lock_guard<std::mutex> lock(_mtx);
        _write_time(lg);
        _fs << text << _line_tag;
        ++_line;
        return _fs.good() ? error_ok : error_io;
    }

    error logger::line(log_e lg, std::string_view text)
    {
        if (!_fs.good())
            return error_state;

        if (lg < _lg)
            return error_ok;

        std::lock_guard<std::mutex> lock(_mtx);
        _write_time(lg);

        _fs.write(text.data(), text.length());
        _fs << _line_tag;
        ++_line;
        return _fs.good() ? error_ok : error_io;
    }

    error logger::line(log_e lg, const char * text, int32_t length)
    {
        if (!_fs.good())
            return error_state;

        if (lg < _lg)
            return error_ok;

        std::lock_guard<std::mutex> lock(_mtx);
        _write_time(lg);
        _fs.write(text, length > 0 ? length : strnlen(text, 1024));
        _fs << _line_tag;
        ++_line;
        return _fs.good() ? error_ok : error_io;
    }

    error logger::_write_time(log_e lg)
    {
        if (!_fs.good())
            return error_state;

        if (_line_curr == _line)
            return error_ok;

        _line_curr = _line;

        timeb tb;
        ftime(&tb);
        char text[64];

        tm tm;
        localtime_s(&tm, &tb.time);

        const char * level[] = { "dbg", "inf", "act", "war", "err" };
        int32_t nchars = _snprintf_s(text, 64, "[%04d-%02d-%02d %02d:%02d:%02d %03d][pid:%-6d][tid:%-6d]%s ",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec,
            tb.millitm,
            core::process_id(),
            core::thread_id(),
            level[std::clamp((int32_t)lg, 0, (int32_t)std::size(level))]);

        _fs.write(text, nchars);
        return error_ok;
    }

    logger_stream logger::d() { return logger_stream(*this, log_dbg); }
    logger_stream logger::i() { return logger_stream(*this, log_inf); }
    logger_stream logger::a() { return logger_stream(*this, log_act); }
    logger_stream logger::w() { return logger_stream(*this, log_war); }
    logger_stream logger::e() { return logger_stream(*this, log_err); }

#ifdef _DEBUG
    logger_stream logger::dbg() { return global_logger::instance()[log_dbg]; }
#endif
    logger_stream logger::inf() { return global_logger::instance()[log_inf]; }
    logger_stream logger::act() { return global_logger::instance()[log_act]; }
    logger_stream logger::war() { return global_logger::instance()[log_war]; }
    logger_stream logger::err() { return global_logger::instance()[log_err]; }

    std::shared_ptr<logger> __global_logger;

    global_logger::global_logger(std::string path, log_e lg)
    {
        std::cout << "log file " << path << std::endl;
        __global_logger = std::make_shared<logger>(path, lg);
    }

    global_logger::~global_logger()
    {
        __global_logger.reset();
    }

    logger & global_logger::instance()
    {
        return *__global_logger.get();
    }
}
