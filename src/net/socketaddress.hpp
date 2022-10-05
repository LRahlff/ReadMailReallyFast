#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>

#include <functional>

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
    socketaddr();

    template <typename T, typename std::enable_if<has_field<T>::value, T>::type * = nullptr>
    explicit socketaddr(T *other) : addr{}, len{} {
        if (other->*(family_map<T>::sa_family_field) != family_map<T>::sa_family) {
            throw netio_exception("Address family mismatch in sockaddr structure.");
        }

        memcpy(&addr, other, sizeof(T));
        len = sizeof(T);
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
        switch(rhs->sa_family) {
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

};

}
