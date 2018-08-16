#pragma once
#include "event.h"

namespace core
{
	class timer
	{
	public:
        timer();
		timer(std::chrono::milliseconds period);
		~timer();

		timer(const timer & another) = delete;

        void setPeriod(std::chrono::milliseconds period) { _period = period; }
        bool running() const;

        void start();
		void start(std::chrono::milliseconds period);
		void stop();

		void callback();

	public:
		core::event<void(timer &, int64_t)> tick;

	private:
        std::shared_ptr<void> _context;
		std::atomic<bool> _started = false;
		std::chrono::milliseconds _period = 1000ms;
		int64_t _tick = 0;
	};
}
