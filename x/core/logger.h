#pragma once

#include "string.h"

namespace core
{
    // core::logger_fps(__FILE__, __LINE__, 1000)
    struct logger_period
    {
        logger_period(const char * file, int line, std::chrono::milliseconds period = 1s, size_t count = 1) : _t(file, line), _period(period), _count(count)
        {

        }

        bool ok() const;

        std::tuple<const char *, int> _t;
        std::chrono::milliseconds _period;
        size_t _count = 1000;
    };

    struct logger_fps : public logger_period
    {
        logger_fps(const char * file, int line, int fps) : logger_period(file, line, 1s, fps) {}
    };
}

// 控制输出频率
namespace std
{
	inline std::u8ostream & operator<<(std::u8ostream & _Ostr, const core::logger_period & fps)
	{
		if (_Ostr.good())
			fps.ok() ? _Ostr.setstate(std::ios_base::goodbit) : _Ostr.setstate(std::ios_base::eofbit);
		return _Ostr;
	}

    inline std::u8ostream & operator<<(std::u8ostream && _Ostr, const core::logger_period & fps)
    {
        if (_Ostr.good())
            fps.ok() ? _Ostr.setstate(std::ios_base::goodbit) : _Ostr.setstate(std::ios_base::eofbit);
        return _Ostr;
    }
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
        null_logger_stream & operator << (T && v) { return *this; }
    };

    class logger_stream : public std::u8ostream, private std::u8streambuf
    {
    public:
        logger_stream(class logger & logger, log_e lg);
        logger_stream(const logger_stream & another);
        ~logger_stream();

        void set_lg(log_e lg);

		std::u8streambuf::int_type overflow(std::u8streambuf::int_type ch) override;
        std::streamsize xsputn(const char8_t * s, std::streamsize n) override;
        int sync() override;
        std::u8ostream & os() { return *this; }

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
        logger(std::u8string path, log_e lg);
        logger(logger && another) = default;
        ~logger();

        void set_lg(log_e lg) { _lg = lg; }
        log_e lg() const { return _lg; }
        void set_proxy(void * opaque, std::function<void(void * opaque, uint32_t level, uint64_t time_ms, uint32_t pid, uint32_t tid, const char8_t * data, uint32_t length)> proxy) { _proxy_opaque = opaque;  _proxy = proxy; }
        error open(const std::u8string & path);
        void close();
        void flush();

        error log_to_buffer(uint32_t pid, uint32_t tid, log_e lg, const std::u8string & text);

        error write(log_e lg, const std::u8string & text);

        error line(log_e lg, const std::u8string & text);
        error line(log_e lg, const char8_t * text, int32_t length = -1);

        operator std::fstream & () { return _fs; }

        logger_stream operator[](log_e lg)
        {
            return logger_stream(*this, lg);
        }

    public:
#ifdef _DEBUG
        // dbg logger 仅存在于 debug build.
        static logger_stream dbg();
#else
        static null_logger_stream dbg() { return null_logger_stream{}; }
#endif
        static logger_stream inf();
        static logger_stream act();
        static logger_stream war();
        static logger_stream err();

    private:
        void writeThread();

    private:
        std::thread _thread;
		std::mutex _mtx_write;
        std::condition_variable _cond;
#ifdef _DEBUG
		std::mutex _mtx_notify;
		std::condition_variable _cond_notify;
#endif

        std::atomic<bool> _finish = false;
        std::atomic_bool _console = false;
        std::atomic_bool _debug_output = false;

        log_e _lg = log_e::log_dbg;
        std::u8string _line_tag = u8"\r\n";
        std::fstream _fs;
        void * _proxy_opaque = nullptr;
        std::function<void(void * opaque, uint32_t level, uint64_t time_ms, uint32_t pid, uint32_t tid, const char8_t * data, uint32_t length)> _proxy;
        struct log_item
        {
            log_e lg;
            uint64_t time_ms;
            uint32_t pid;
            uint32_t tid;
            std::u8string text;
        };
        std::list<log_item> _log_list;
        int64_t _line = 0;
        int64_t _line_curr = -1;
    };

    class global_logger
    {
    public:
        global_logger();
        global_logger(const std::u8string & path, log_e lg);
        ~global_logger();

        static void start(const std::u8string & path, log_e lg);
        static void stop();

        static void set_proxy(void * opaque, std::function<void(void * opaque, uint32_t level, uint64_t time_ms, uint32_t pid, uint32_t tid, const char8_t * data, uint32_t length)> proxy);
        static logger & ref();
        static logger & instance();

    private:
        std::shared_ptr<logger> _logger;
    };

    inline logger_stream inf() { return global_logger::ref()[log_inf]; }
    inline logger_stream act() { return global_logger::ref()[log_act]; }
    inline logger_stream war() { return global_logger::ref()[log_war]; }
    inline logger_stream err() { return global_logger::ref()[log_err]; }
}
