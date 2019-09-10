#pragma once
#include "event.h"

namespace core
{
	class timer_context
	{
	public:
		virtual ~timer_context() = default;
		virtual uint64_t create_timer_id() = 0;
		virtual void add(uint64_t id, class timer * ptimer, std::chrono::milliseconds period) = 0;
		virtual void add_single(uint64_t id, std::shared_ptr<core::object> object, std::chrono::milliseconds delay, std::function<void()> callback) = 0;
		virtual void kill(uint64_t id) = 0;
		virtual void kill_single(uint64_t id) = 0;
		virtual void trigger(uint64_t id) = 0;
		virtual void trigger_single(uint64_t id) = 0;
	};
	
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
		static timer_context & get_context();
		static uint64_t invoke(std::shared_ptr<core::object> object, std::chrono::milliseconds delay, std::function<void()> callback);
		static void cancel(uint64_t id);

	private:
        timer_context & _context = get_context();
		uint64_t _id = get_context().create_timer_id();
		std::atomic<bool> _started = false;
		std::chrono::milliseconds _period = 1000ms;
		int64_t _tick = 0;
	};
}
