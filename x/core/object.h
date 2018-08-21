#pragma once

namespace core
{
    class object;
    class invokable_helper
    {
    public:
        invokable_helper();
        ~invokable_helper();

        invokable_helper & ref();
        error add(std::shared_ptr<object> invoker, std::function<void()>);

        error trigger();
    private:
        uint32_t _id = 0;
        void * _thread = nullptr;
        typedef std::map<std::weak_ptr<object>, std::list<std::function<void()>>, std::owner_less<std::weak_ptr<object>>> invoker_map;
        invoker_map _invokers;
        std::mutex _mtx;
    };

    class object : public std::enable_shared_from_this<object>
    {
    public:
        object() = default;
        virtual ~object() = default;

        template<typename T>
        std::shared_ptr<T> share_ref()
        {
            return std::dynamic_pointer_cast<T>(shared_from_this());
        }

        template<typename T>
        std::weak_ptr<T> weak_ref()
        {
            return std::dynamic_pointer_cast<T>(shared_from_this());
        }

        uint64_t id() const { return _id; }
        error invoke(std::function<void()>);

    public:
        static uint64_t create_objectid();
        static invokable_helper & invokable_get_helper();

    protected:
        uint64_t _id = create_objectid();
        invokable_helper & _invoker_helper = invokable_get_helper();
    };
}
