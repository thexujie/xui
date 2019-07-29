#pragma once

namespace core::network
{
    typedef int32_t port_t;

    struct netaddr
    {
        std::array<uint8_t, 4> v4;
    };

    std::vector<netaddr> host2addrs(const std::u8string & hostname);
}
