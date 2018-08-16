#pragma once

namespace core
{
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
    };
}
