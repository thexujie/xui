#pragma once

namespace std
{
	template<class _Ret,
		size_t... _Ix,
		class _Cv_FD,
		class _Cv_tuple_TiD,
		class _Untuple> inline
		auto _Weak_Call_binder(_Invoker_ret<_Ret>, index_sequence<_Ix...>,
			_Cv_FD& _Obj, _Cv_tuple_TiD& _Tpl, _Untuple&& _Ut)
		-> decltype(_Invoker_ret<_Ret>::_Call(_Obj, _Fix_arg(_STD get<_Ix>(_Tpl), _STD move(_Ut))...))
	{	// bind() and bind<R>() invocation
		return (_Invoker_ret<_Ret>::_Call(_Obj, _Fix_arg(_STD get<_Ix>(_Tpl), _STD move(_Ut))...));
	}

	template<class _Ret, class _Fx, class _Tx, class... _Types>
	class _WeakBinder : public _Binder_result_type<_Ret, _Fx>::type
	{
		// wrap bound callable object and arguments
	private:
		typedef std::shared_ptr<_Tx> _ShptrT;
		typedef std::weak_ptr<_Tx> _WeakPtrT;
		typedef std::index_sequence_for<_ShptrT, _Types...> _Seq;
		typedef std::decay_t<_Fx> _First;
		typedef std::tuple<_ShptrT, std::decay_t<_Types>...> _Second;

		_WeakPtrT _MyWptr;
		std::_Compressed_pair<_First, _Second> _Mypair;

	public:
		explicit _WeakBinder(_Fx && _Func, _ShptrT _Shp, _Types &&... _Args)
			: _MyWptr(_Shp), _Mypair(std::_One_then_variadic_args_t(), std::forward<_Fx>(_Func), _ShptrT(), std::forward<_Types>(_Args)...)
		{
		}


		//template<class... _Unbound>
		//auto operator()(_Unbound &&... _Unbargs) -> decltype(std::_Weak_Call_binder(std::_Invoker_ret<_Ret>(), _Seq(), _Mypair._Get_first(), _Mypair._Get_second(),
		//	std::forward_as_tuple(std::forward<_Unbound>(_Unbargs)...)))
		//{
		//	auto & _second = _Mypair._Get_second();
		//	std::get<0>(_second) = _MyWptr.lock();
		//		return (std::_Weak_Call_binder(std::_Invoker_ret<_Ret>(), _Seq(), _Mypair._Get_first(), _Mypair._Get_second(), std::forward_as_tuple(std::forward<_Unbound>(_Unbargs)...)));
		//}

		// 不处理任何返回值
		template<class... _Unbound>
		void operator()(_Unbound &&... _Unbargs)
		{
			auto & _second = _Mypair._Get_second();
			std::get<0>(_second) = _MyWptr.lock();
			if(std::get<0>(_second))
			{
				std::_Weak_Call_binder(std::_Invoker_ret<_Ret>(), _Seq(), _Mypair._Get_first(), _Mypair._Get_second(), std::forward_as_tuple(std::forward<_Unbound>(_Unbargs)...));
				std::get<0>(_second).reset();
			}
		}
	};

	template<class _Fx, class _Tx, class... _Types>
	inline _WeakBinder<std::_Unforced, _Fx, _Tx, _Types...>
		weak_bind(_Fx && _Func, std::shared_ptr<_Tx> _Shp, _Types &&... _Args)
	{
		return (_WeakBinder<std::_Unforced, _Fx, _Tx, _Types...>(std::forward<_Fx>(_Func), _Shp, std::forward<_Types>(_Args)...));
	}

    template<class _Rx, class _Ty>
    class weak_binder
    {
    public:
        typedef std::_Mem_fn<_Rx _Ty::*> FunT;
        typedef _Rx RetT;

        weak_binder(const weak_binder & another) : _wptr(another._wptr), _fun(another._fun) {}
        weak_binder(FunT fun, std::shared_ptr<_Ty> wptr) : _wptr(wptr), _fun(fun) {}
        template<typename FarT>
        weak_binder(FunT fun, std::shared_ptr<FarT> wptr) : _wptr(std::dynamic_pointer_cast<_Ty>(wptr)), _fun(fun) {}

        template<class... _Types>
        auto operator()(_Types&&... _Args) const-> decltype(_fun(_Ty(), std::forward<_Types>(_Args)...))
        {
            auto ptr = std::dynamic_pointer_cast<_Ty>(_wptr.lock());
            if constexpr (std::is_void<decltype(_fun(_Ty(), std::forward<_Types>(_Args)...))>::value)
            {
                if (ptr)
                    _fun(*ptr, std::forward<_Types>(_Args)...);
            }
            else
            {
                if (!ptr)
                    return {};

                return _fun(*ptr, std::forward<_Types>(_Args)...);
            }
        }

    private:
        std::weak_ptr<_Ty> _wptr;
        FunT _fun;
    };

}

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