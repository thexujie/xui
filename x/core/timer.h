#pragma once
#include "event.h"

namespace core
{
	struct timer_context;
	class timer
	{
	public:
        timer();
		timer(std::chrono::milliseconds period);
		~timer();

		timer(const timer & another) = delete;
		uint64_t id() const { return _id; }

        void setPeriod(std::chrono::milliseconds period) { _period = period; }
        bool running() const;

        void start();
		void start(std::chrono::milliseconds period);
		void stop();

		void callback();

	public:
		core::event<void(timer &, int64_t)> tick;

	public:
		static uint64_t invoke(std::shared_ptr<core::object> object, std::chrono::milliseconds delay, std::function<void()> callback);
		static void cancel(uint64_t id);

	private:
		timer_context & _context;
		uint64_t _id = 0;
		std::atomic<bool> _started = false;
		std::chrono::milliseconds _period = 1000ms;
		int64_t _tick = 0;
	};
}
