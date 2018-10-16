#include "stdafx.h"
#include "addr.h"

#include "win32/win32.h"
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

namespace core::network
{
    std::vector<netaddr> host2addrs(const std::string & hostname)
    {
        struct addrinfo hints = {};
        hints.ai_flags = 0/*AI_NUMERICHOST*/;
        hints.ai_family = AF_INET;
        hints.ai_socktype = 0;
        hints.ai_protocol = 0;
        struct addrinfo * res = nullptr;
        int err = getaddrinfo(hostname.c_str(), NULL, &hints, &res);
        if (!err && res)
        {
            std::vector<netaddr> vec;
            struct addrinfo * ai = res;
            while (ai)
            {
                if (ai->ai_addrlen == sizeof(sockaddr_in))
                {
                    const sockaddr_in & si = *reinterpret_cast<const sockaddr_in *>(ai->ai_addr);
                    netaddr addr = { { si.sin_addr.S_un.S_un_b.s_b1, si.sin_addr.S_un.S_un_b.s_b2, si.sin_addr.S_un.S_un_b.s_b3, si.sin_addr.S_un.S_un_b.s_b4 } };
                    vec.emplace_back(addr);
                }
                ai = ai->ai_next;
            }
            freeaddrinfo(res);
            return vec;
        }
        else
            return {};
    }
}
