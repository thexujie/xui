#include "stdafx.h"
#include "platform.h"

namespace core
{
	static void CALLBACK InvokerAPCCallBack(ULONG_PTR dwParam)
	{
		if (dwParam)
			((invoke_helper *)dwParam)->trigger();
	}

	invoke_helper::invoke_helper()
	{
		_id = core::thread_id();
	}
	invoke_helper::~invoke_helper()
	{
		std::unique_lock<std::mutex> lock(_mtx);
		if (_thread)
		{
			CloseHandle(_thread);
			_thread = nullptr;
			_id = 0;
		}
		lock.unlock();
	}

	invoke_helper & invoke_helper::ref()
	{
		if (_id != GetCurrentThreadId())
			throw 0;

		return *this;
	}

	bool invoke_helper::can_safe_invoke() const
	{
		return core::thread_id() == _id;
	}

	void invoke_helper::check_invoke()
	{
		if (core::thread_id() != _id)
			throw core::exception(core::e_invalid_operation);
	}

	error invoke_helper::add(std::shared_ptr<object> invoker, std::function<void()> fun)
	{
		if (!_id)
			return e_state;
		std::lock_guard<std::mutex> lock(_mtx);
		if (!_thread)
		{
			_thread = OpenThread(THREAD_SET_CONTEXT, FALSE, _id);
			if (!_thread)
			{
				logger::err() << __FUNCTION__" OpenThread" << platform::win32::winerr_str(GetLastError());
			}
		}
		if (!_thread)
			return e_state;

		_invokers[invoker].push_back(fun);
		::QueueUserAPC(InvokerAPCCallBack, (HANDLE)_thread, (ULONG_PTR)this);
		return ok;
	}

	error invoke_helper::add(std::shared_ptr<object> invoker, std::shared_ptr<invoke_task> task)
	{
		if (!_id)
			return e_state;
		std::lock_guard<std::mutex> lock(_mtx);
		if (!_thread)
		{
			_thread = OpenThread(THREAD_SET_CONTEXT, FALSE, _id);
			if (!_thread)
			{
				logger::err() << __FUNCTION__" OpenThread" << platform::win32::winerr_str(GetLastError());
			}
		}
		if (!_thread)
			return e_state;

		_tasks[invoker].push_back(task);
		::QueueUserAPC(InvokerAPCCallBack, (HANDLE)_thread, (ULONG_PTR)this);
		return ok;
	}

	error invoke_helper::trigger()
	{
		if (_id != GetCurrentThreadId())
			return e_state;

		invoker_map invokers;
		task_map tasks;
		while (true)
		{
			{
				std::lock_guard<std::mutex> lock(_mtx);
				invokers = std::move(_invokers);
				tasks = std::move(_tasks);
			}

			if (invokers.empty() && tasks.empty())
				break;

			for (invoker_map::iterator iter = invokers.begin(); iter != invokers.end(); ++iter)
			{
				auto invoker = iter->first.lock();
				if (!invoker)
					continue;

				for (auto & fun : iter->second)
					fun();
			}

			for (task_map::iterator iter = tasks.begin(); iter != tasks.end(); ++iter)
			{
				auto invoker = iter->first.lock();
				if (!invoker)
					continue;

				for (auto & task : iter->second)
					task->trigger();
			}

		}
		return ok;
	}
}