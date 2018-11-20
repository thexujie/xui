#include "stdafx.h"
#include "timer.h"
#include "win32/win32.h"

namespace core
{
	template <typename T>
	static bool is_weak_ptr_uninitialized(std::weak_ptr<T> const& weak) {
		using wt = std::weak_ptr<T>;
		return !weak.owner_before(wt{}) && !wt{}.owner_before(weak);
	}

	static void CALLBACK TimerCallBack(HWND, UINT, UINT_PTR timerId, DWORD);
	static void CALLBACK TimerCallBackSingle(HWND, UINT, UINT_PTR timerId, DWORD);

	const wchar_t _timerHWND_ClassName[] = L"timer.{2A84CC0A-7D4F-42AB-9311-DEA336A1F289}";

	struct timer_context
	{
		std::mutex _mtx;
		HWND _timerHWND = NULL;
		bool _timerHWND_Registed = false;

		std::map<uint64_t, timer *> timers;
		std::map<uint64_t, std::pair<std::weak_ptr<object>, std::function<void()>>> singleTimers;

		HWND hwnd()
		{
			if(!_timerHWND)
			{
				HINSTANCE hInstance = GetModuleHandle(NULL);
				if(!_timerHWND_Registed)
				{
					WNDCLASSEXW wcex = { sizeof(WNDCLASSEXW) };
					wcex.style = CS_HREDRAW | CS_VREDRAW;
					wcex.lpfnWndProc = DefWindowProcW;
					wcex.cbClsExtra = 0;
					wcex.cbWndExtra = 0;
					wcex.hInstance = hInstance;
					wcex.hIcon = NULL;
					wcex.hCursor = NULL;
					wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
					wcex.lpszMenuName = NULL;
					wcex.lpszClassName = _timerHWND_ClassName;
					wcex.hIconSm = NULL;

					RegisterClassExW(&wcex);
				}
				_timerHWND = CreateWindowExW(0, _timerHWND_ClassName, L"_timerHWND", WS_OVERLAPPEDWINDOW,
					0, 0, CW_DEFAULT, CW_DEFAULT, HWND_MESSAGE, NULL, hInstance, NULL);
			}
			return _timerHWND;
		}

		void add(uint64_t id, timer * ptimer, std::chrono::milliseconds period)
		{
			if(!ptimer)
				return;

			std::lock_guard<std::mutex> lock(_mtx);
			::SetTimer(hwnd(), (UINT_PTR)id, (UINT)period.count(), TimerCallBack);
			timers[id] = ptimer;
		}

		void addSingle(uint64_t id, std::shared_ptr<core::object> object, std::chrono::milliseconds delay, std::function<void()> callback)
		{
			if(!id)
				return;

			std::lock_guard<std::mutex> lock(_mtx);
			::SetTimer(hwnd(), (UINT_PTR)id, (UINT)delay.count(), TimerCallBackSingle);
			singleTimers[id] = std::make_pair(object, callback);
		}

		void kill(uint64_t id)
		{
			if(!id)
				return;

			std::lock_guard<std::mutex> lock(_mtx);
			if(!_timerHWND)
				return;

			auto iter = timers.find(id);
			if(iter != timers.end())
			{
				timers.erase(iter);
				::KillTimer(_timerHWND, id);
			}
		}

		void killSingle(uint64_t id)
		{
			if(!id)
				return;

			std::lock_guard<std::mutex> lock(_mtx);
			if(!_timerHWND)
				return;

			auto iter = singleTimers.find(id);
			if(iter != singleTimers.end())
			{
				singleTimers.erase(iter);
				::KillTimer(_timerHWND, id);
			}
		}

		void trigger(uint64_t id)
		{
			timers[id]->callback();
		}

		void triggerSingle(uint64_t id)
		{
			std::lock_guard<std::mutex> lock(_mtx);
			auto iter = singleTimers.find(id);
			if(iter != singleTimers.end())
			{
				if(is_weak_ptr_uninitialized(iter->second.first))
					iter->second.second();
				else
				{
					auto shp = iter->second.first.lock();
					if(shp)
						iter->second.second();
				}
				::KillTimer(_timerHWND, id);
				singleTimers.erase(iter);
			}
		}
	};

    static const uint64_t _timer_one = 1;
    static std::atomic_uint64_t __timer_id = 1;
    static thread_local std::unique_ptr<timer_context> _timer_context;
    static timer_context & __timer_context_get()
    {
        if (!_timer_context)
            _timer_context.reset(new timer_context());
        return *_timer_context;
    }

	static void CALLBACK TimerCallBack(HWND, UINT, UINT_PTR timerId, DWORD)
	{
        if(_timer_context)
		    _timer_context->trigger(timerId);
	}
	static void CALLBACK TimerCallBackSingle(HWND, UINT, UINT_PTR timerId, DWORD)
	{
        if(_timer_context)
		    _timer_context->triggerSingle(timerId);
	}

    timer::timer():_context(__timer_context_get()), _id(__timer_id.fetch_add(_timer_one))
    {
	    
    }

	timer::timer(std::chrono::milliseconds period): _context(__timer_context_get()), _id(__timer_id.fetch_add(_timer_one)), _period(period)
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
		uint64_t id = __timer_id.fetch_add(_timer_one);
        __timer_context_get().addSingle(id, object, delay, callback);
		return id;
    }

	void timer::cancel(uint64_t id)
    {
        if(_timer_context)
		    _timer_context->kill(id);
    }
}
