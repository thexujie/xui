#pragma once

#include "core/string.h"

inline std::basic_ostream<char, std::char_traits<char>> & __fixed_op(std::basic_ostream<char, std::char_traits<char>> & _Ostr, const char * _Val)
{
    return _Ostr << _Val;
}

inline std::basic_ostream<char, std::char_traits<char>> & __fixed_op(std::basic_ostream<char, std::char_traits<char>> && _Ostr, const char * _Val)
{
    return _Ostr << _Val;
}

inline std::basic_ostream<char, std::char_traits<char>> & operator<<(std::basic_ostream<char, std::char_traits<char>> && _Ostr, const char * _Val)
{
    if (!_Val)
        return __fixed_op(_Ostr, "null");
    else
        return __fixed_op(_Ostr, _Val);
}

inline std::basic_ostream<char, std::char_traits<char>> & operator<<(std::basic_ostream<char, std::char_traits<char>> & _Ostr, const char * _Val)
{
    if (!_Val)
        return __fixed_op(_Ostr, "null");
    else
        return __fixed_op(_Ostr, _Val);
}

inline std::basic_ostream<char, std::char_traits<char>> & operator<<(std::basic_ostream<char, std::char_traits<char>> && _Ostr, const wchar_t * _Val)
{
    return _Ostr << core::wstr_u8str(_Val);
}

inline std::basic_ostream<char, std::char_traits<char>> & operator<<(std::basic_ostream<char, std::char_traits<char>> & _Ostr, const wchar_t * _Val)
{
    return _Ostr << core::wstr_u8str(_Val);
}

namespace core
{
    enum log_e
    {
        log_dbg = 0,
        log_inf,
        log_act,
        log_war,
        log_err,
    };

    class null_logger_stream
    {
    public:
        null_logger_stream() = default;
        ~null_logger_stream() = default;

        template<typename T>
        null_logger_stream & operator <<(T && v) { return *this; }
    };

    class logger_stream : public std::ostream, private std::streambuf
    {
    public:
        logger_stream(class logger & logger, log_e lg);
        logger_stream(const logger_stream & another);
        ~logger_stream();

        void set_lg(log_e lg) { _lg_curr = lg; }

        int overflow(int ch);
        std::streamsize xsputn(const char * s, std::streamsize n);
        int sync();
        std::ostream & os() { return *this; }

    private:
        log_e _lg_curr = log_e::log_dbg;
        log_e _lg = log_e::log_inf;
        class logger & _logger;
        std::stringbuf _buffer;
    };

    class logger
    {
    public:
        logger();
        logger(std::string path, log_e lg);
        logger(logger && another) = default;
        ~logger();

        void set_lg(log_e lg) { _lg = lg; }
        log_e lg() const { return _lg; }

        error open(std::string path);
        void close();
        void flush();

        error write(log_e lg, std::string text);
        error write(log_e lg, std::string_view text);

        error line(log_e lg);
        error line(log_e lg, std::string text);
        error line(log_e lg, std::string_view text);
        error line(log_e lg, const char * text, int32_t length = -1);

        operator std::fstream &() { return _fs; }

        logger_stream operator[](log_e lg)
        {
            return logger_stream(*this, lg);
        }

        logger_stream d();
        logger_stream i();
        logger_stream a();
        logger_stream w();
        logger_stream e();
    public:
#ifdef _DEBUG
        // dbg logger ½ö´æÔÚÓÚ debug build.
        static logger_stream dbg();
#else
        static null_logger_stream dbg() { return null_logger_stream{}; }
#endif
        static logger_stream inf();
        static logger_stream act();
        static logger_stream war();
        static logger_stream err();

    private:
        error _write_time(log_e lg);

    private:
        log_e _lg = log_e::log_dbg;
        std::mutex _mtx;
        std::string _line_tag = "\r\n";
        std::fstream _fs;
        int64_t _line = 0;
        int64_t _line_curr = -1;
    };

    class global_logger
    {
    public:
        global_logger(std::string path, log_e lg);
        ~global_logger();

        static logger & instance();
    };

    inline void glog_helper(logger_stream & stream) {}

    template<typename Head, typename... Tail>
    void glog_helper(logger_stream & stream, Head const & head, Tail &&... tail)
    {
        stream << head;
        glog_helper(stream, std::forward<Tail>(tail)...);
    }

    template<log_e lg, typename ...ArgsT>
    void glog(ArgsT && ...args)
    {
        core::logger_stream stream = global_logger::instance()[lg];
        glog_helper(stream, std::forward<ArgsT>(args)...);
    }

    template<typename ...ArgsT>
    void glog_dbg(ArgsT && ...args)
    {
        glog<log_dbg, ArgsT...>(std::forward<ArgsT>(args)...);
    }

    template<typename ...ArgsT>
    void glog_inf(ArgsT && ...args)
    {
        glog<log_inf, ArgsT...>(std::forward<ArgsT>(args)...);
    }

    template<typename ...ArgsT>
    void glog_act(ArgsT && ...args)
    {
        glog<log_act, ArgsT...>(std::forward<ArgsT>(args)...);
    }

    template<typename ...ArgsT>
    void glog_war(ArgsT && ...args)
    {
        glog<log_war, ArgsT...>(std::forward<ArgsT>(args)...);
    }

    template<typename ...ArgsT>
    void glog_err(ArgsT && ...args)
    {
        glog<log_err, ArgsT...>(std::forward<ArgsT>(args)...);
    }
}
