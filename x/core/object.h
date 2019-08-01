#pragma once

namespace core
{
    class object;

    class invoke_task
    {
    public:
        virtual ~invoke_task() = default;
        virtual error trigger() = 0;
    };

    template<typename T>
    class invoke_task_promise : public invoke_task
    {
    public:
        invoke_task_promise(std::function<T()> fun) : _fun(fun) {}

        error trigger()
        {
            if constexpr (std::is_void_v<T>)
            {
                _fun();
                _promise.set_value();
            }
            else
                _promise.set_value(_fun());
            return ok;
        }

        std::future<T> get_future()
        {
            return _promise.get_future();
        }

        std::promise<T> _promise;
        std::function<T()> _fun;
    };

    class invoke_helper
    {
    public:
        invoke_helper();
        ~invoke_helper();

        invoke_helper & ref();

        uint32_t thread_id() const { return _id; }
        void * thread_handle() const;

        bool can_safe_invoke() const;
        void check_invoke();

        error add(std::shared_ptr<object> invoker, std::function<void()>);
        error add(std::shared_ptr<object> invoker, std::shared_ptr<invoke_task> task);
        error trigger();
    private:
        uint32_t _id = 0;
        mutable void * _thread = nullptr;
        mutable std::mutex _mtx;
        typedef std::map<std::weak_ptr<object>, std::vector<std::function<void()>>, std::owner_less<std::weak_ptr<object>>> invoker_map;
        typedef std::map<std::weak_ptr<object>, std::vector<std::shared_ptr<invoke_task>>, std::owner_less<std::weak_ptr<object>>> task_map;
        invoker_map _invokers;
        task_map _tasks;
    };

    class object : public std::enable_shared_from_this<object>
    {
    public:
        object() = default;
        virtual ~object() = default;

        template<typename T>
        std::shared_ptr<T> share_ref() const
        {
            return std::dynamic_pointer_cast<T>(const_cast<object *>(this)->shared_from_this());
        }

        template<typename T>
        std::weak_ptr<T> weak_ref() const
        {
            return std::dynamic_pointer_cast<T>(const_cast<object *>(this)->shared_from_this());
        }

        uint64_t id() const { return _id; }

        bool can_safe_invoke() const
        {
            return _invoke_helper.can_safe_invoke();
        }

        void check_invoke()
        {
            return _invoke_helper.check_invoke();
        }

        error invoke(std::function<void()> fun)
        {
            return _invoke_helper.add(shared_from_this(), fun);
        }

        template<typename T>
        std::future<T> dispatch(std::function<T()> fun)
        {
            auto task = std::make_shared<invoke_task_promise<T>>(fun);
            _invoke_helper.add(shared_from_this(), task);
            return task->get_future();
        }

        template<typename FunT>
        decltype(std::declval<FunT>()()) async(FunT fun)
        {
            if (can_safe_invoke())
                return fun();

            typedef decltype(std::declval<FunT>()()) T;
            auto task = std::make_shared<invoke_task_promise<T>>(fun);
            _invoke_helper.add(shared_from_this(), task);
            return task->get_future().get();
        }

        template<typename T>
        bool is_type_of()
        {
            return dynamic_cast<T *>(this) != nullptr;
        }

    public:
        static uint64_t create_objectid();
        static invoke_helper & get_invoke_helper();

    protected:
        uint64_t _id = create_objectid();
        invoke_helper & _invoke_helper = get_invoke_helper();
    };
}
