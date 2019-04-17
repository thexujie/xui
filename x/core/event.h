#pragma once

namespace core
{
    enum class event_flag
    {
        none = 0,
        rejected = 0x0001,
    };

    typedef bitflag<event_flag> event_flags;

	template<typename _Fty>
	class event final
	{
		enum flags
		{
			none = 0,
			expired = 0x0001,
		};

		struct callback
		{
			std::function<_Fty> function;
			int32_t flag;
		};

	public:
		void operator += (std::function<_Fty> func)
		{
			_functions.push_back(callback{ func });
		}

		void operator -= (std::function<_Fty> func)
		{
			auto iter = std::find(std::begin(_functions), std::end(_functions), [](const auto & c) { return c.function == func; });
			if(iter != _functions.end())
				_functions.erase(iter);
		}

		template<typename T>
		void operator-=(const T & fun)
		{
			std::function<_Fty> func = fun;
			auto addr = func.template target<T>();
			if(!addr)
				return;

			for(auto & c : _functions)
			{
				auto & t = c.function.target_type();
				if(t != typeid(T))
					continue;

				auto a0 = c.function.template target<T>();
				if(!a0)
					continue;

				if(std::memcmp(a0, addr, sizeof(T)) != 0)
					continue;

				c.flag |= flags::expired;
			};
		}

		template<typename ...ArgsT>
		void operator ()(ArgsT && ...args)
		{
			for(auto & c : _functions)
			{
				if(c.flag & flags::expired)
					continue;

				c.function(std::forward<ArgsT>(args)...);
			}

			_functions.erase(std::remove_if(std::begin(_functions), std::end(_functions), [](const auto & c) { return !!(c.flag & flags::expired); }), _functions.end());
		}
	public:
		std::list<callback> _functions;
	};
}