#pragma once

#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>

#include <functional>
#include <string>
#include <sstream>

#include "macros.hpp"
#include "net/netio_exception.hpp"

#ifdef __linux__
#include <linux/netlink.h>
#endif

namespace rmrf::net {

template <typename T>
struct family_map {};

template <>
struct family_map<sockaddr_in> {
    static constexpr int sa_family = AF_INET;
    static constexpr auto sa_family_field = &sockaddr_in::sin_family;
};

template <>
struct family_map<sockaddr_in6> {
    static constexpr int sa_family = AF_INET6;
    static constexpr auto sa_family_field = &sockaddr_in6::sin6_family;
};

template <>
struct family_map<sockaddr_un> {
    static constexpr int sa_family = AF_UNIX;
    static constexpr auto sa_family_field = &sockaddr_un::sun_family;
};

template <>
struct family_map<sockaddr_nl> {
    static constexpr int sa_family = AF_NETLINK;
    static constexpr auto sa_family_field = &sockaddr_nl::nl_family;
};

template <typename, typename = void>
struct has_field : std::false_type {};

template <typename T>
struct has_field<T, std::void_t<decltype(family_map<T>::sa_family)>> : std::is_convertible<decltype(family_map<T>::sa_family), int> {};

class socketaddr {
private:
    sockaddr_storage addr;
    socklen_t len;

public:
    socketaddr() = default;

    template <typename T, typename std::enable_if<has_field<T>::value, T>::type * = nullptr>
    explicit socketaddr(T *other) : addr{}, len{} {
        if (other->*(family_map<T>::sa_family_field) != family_map<T>::sa_family) {
            throw netio_exception("Address family mismatch in sockaddr structure.");
        }

        memcpy(&addr, other, sizeof(T));
        len = sizeof(T);
    }

    explicit socketaddr(const sockaddr_storage *other) : addr{}, len{} {
        *this = other;
    }

    explicit socketaddr(const sockaddr_storage &other) : addr{}, len{} {
        *this = &other;
    }

    template <typename T, typename std::enable_if<has_field<T>::value, T>::type * = nullptr>
    explicit socketaddr(const T& other) : addr{}, len{} {
        if (other.*(family_map<T>::sa_family_field) != family_map<T>::sa_family) {
            throw netio_exception("Address family mismatch in sockaddr structure.");
        }

        memcpy(&addr, &other, sizeof(T));
        len = sizeof(T);
    }

    template <typename T>
    socketaddr& operator=(const T *rhs) {
        if (rhs->*(family_map<T>::sa_family_field) != family_map<T>::sa_family) {
            throw netio_exception("Address family mismatch in sockaddr structure.");
        }

        memcpy(&addr, rhs, sizeof(T));
        len = sizeof(T);

COMPILER_SUPRESS("-Weffc++");
        return *this;
COMPILER_RESTORE("-Weffc++");
    }

    socketaddr& operator=(const sockaddr_storage *rhs) {
        *this = (sockaddr*)rhs;
COMPILER_SUPRESS("-Weffc++");
        return *this;
COMPILER_RESTORE("-Weffc++");
    }

    socketaddr& operator=(sockaddr *rhs) {
        switch (rhs->sa_family) {
        case AF_INET:
            return *this = (sockaddr_in *)rhs;
        case AF_INET6:
            return *this = (sockaddr_in6 *)rhs;
        case AF_UNIX:
            return *this = (sockaddr_un *)rhs;
        case AF_NETLINK:
            return *this = (sockaddr_nl *)rhs;
        default:
            throw netio_exception("Trying to assign unknown address family");
        }
    };

    int family() const {
        return addr.ss_family;
    }

    template <typename T>
    operator T*() const {
        if (addr.ss_family != family_map<T>::sa_family) {
            throw netio_exception("Cannot convert address family of stored sockaddr structure.");
        }

        return (T*)&addr;
    }

    sockaddr* ptr() const {
        return (sockaddr*)&addr;
    }

    socklen_t size() const {
        return len;
    }

    std::string str() const {
        std::ostringstream oss;
        oss << "SocketAddress: ";
        const int buffer_size = 1024;
        char buffer[buffer_size];

        switch (this->family()) {
        case AF_INET:
            inet_ntop(AF_INET, &((sockaddr_in*)&addr)->sin_addr, buffer, buffer_size);
            oss << "IPv4 " << buffer << ":" << ntohs(((sockaddr_in*)&addr)->sin_port);
            break;
        case AF_INET6:
            inet_ntop(AF_INET6, &((sockaddr_in6*)&addr)->sin6_addr, buffer, buffer_size);
            oss << "IPv6 ["<< buffer << "]:" << ntohs(((sockaddr_in6*)&addr)->sin6_port);
            break;
        case AF_UNIX:
            oss << "FileSocket " << ((sockaddr_un*)&addr)->sun_path;
            break;
        case AF_NETLINK: {
            const auto nl_sock_ptr = (sockaddr_nl*) &addr;
            oss << "Netlink g:" << nl_sock_ptr->nl_groups << " p:" << nl_sock_ptr->nl_pad << " pid:" << nl_sock_ptr->nl_pid;
            break;
        }
        default:
            oss << "Unknown Socket Address Type";
            break;
        }

        return oss.str();
    }

};

static inline std::ostream& operator<<(std::ostream& os, const socketaddr& sockaddr) {
    return os << sockaddr.str();
}

}
