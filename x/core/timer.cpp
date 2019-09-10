#include "stdafx.h"
#include "timer.h"

namespace core
{
	class timer_context_impl;
	static thread_local std::shared_ptr<timer_context> _timer_context;
	std::shared_ptr<timer_context> create_timer_context();
	
	core::timer_context & timer::get_context()
	{
		if (!_timer_context)
			_timer_context = create_timer_context();
		return *_timer_context;
	}
	
    timer::timer()
    {
	    
    }

	timer::timer(std::chrono::milliseconds period): _period(period)
	{

	}

	timer::~timer()
	{
        bool _true = true;
		if(_started.compare_exchange_strong(_true, false))
			_context.kill(_id);
	}

    bool timer::running() const
    {
        return _started;
    }

	void timer::start()
	{
        bool _false = false;
		if(!_started.compare_exchange_strong(_false, true))
			return;

		_context.add(_id, this, _period);
	}

	void timer::start(std::chrono::milliseconds period)
	{
        bool _false = false;
        if(!_started.compare_exchange_strong(_false, true))
            return;

		_period = period;
		_context.add(_id, this, _period);
	}

	void timer::stop()
	{
        bool _true = true;
        if(!_started.compare_exchange_strong(_true, false))
            return;

		_context.kill(_id);
        _started = false;
        _tick = 0;
	}

	void timer::callback()
	{
        tick(std::ref(*this), _tick++);
	}

	uint64_t timer::invoke(std::shared_ptr<core::object> object, std::chrono::milliseconds delay, std::function<void()> callback)
    {
		uint64_t id = get_context().create_timer_id();
		get_context().add_single(id, object, delay, callback);
		return id;
    }

	void timer::cancel(uint64_t id)
    {
		get_context().kill(id);
    }
}
