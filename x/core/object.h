#pragma once

namespace core
{
    class object : public std::enable_shared_from_this<object>
    {
    public:
        object() = default;
        virtual ~object() = default;
    };
}
