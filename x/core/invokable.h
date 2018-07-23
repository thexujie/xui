#pragma once

namespace core
{
    class iinvokable
    {
    public:
        virtual ~iinvokable() {}
        virtual error trigger() = 0;
    };

    class invokable_helper
    {
    public:
	    invokable_helper();
	    ~invokable_helper();

        invokable_helper & ref();
        error add(std::weak_ptr<iinvokable> invoker);

        // 只应该在主线程退出时（进程的最后一个线程退出后）调用，其他时候调用结果都是未知的（会 delete mutex）
        void clear();
        error trigger();
    private:
        uint32_t _id = 0;
        void * _thread = nullptr;
        std::list<std::weak_ptr<iinvokable>> * _invokers = nullptr;
        std::mutex * _invoker_mutex = nullptr;
    };

    void invokable_clear();
    invokable_helper & invokable_get_helper();


    template<typename T>
    class invokable : public std::enable_shared_from_this<T>, public iinvokable
    {
    public:
        invokable() = default;
        ~invokable() = default;

        error invoke(std::function<void(void)> func)
        {
            std::lock_guard<std::mutex> lock(_mtxInvokable);
            _invoker_functions.push_back(func);
            _invoker_helper.add(this_weak_ref());
            return error_ok;
        }

        error trigger()
        {
            std::vector<std::function<void()>> invoker_functions;
            {
                std::lock_guard<std::mutex> lock(_mtxInvokable);
                invoker_functions = std::move(_invoker_functions);
            }

            for(auto function : invoker_functions)
                function();
            return error_ok;
        }

    protected:
        std::weak_ptr<iinvokable> this_weak_ref()
        {
            return std::enable_shared_from_this<T>::weak_from_this();
        }

    protected:
        invokable_helper & _invoker_helper = invokable_get_helper();
        std::mutex _mtxInvokable;
        std::vector<std::function<void()>> _invoker_functions;
    };
}
