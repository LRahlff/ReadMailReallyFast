#pragma once

#include <unistd.h>

namespace rmrf::net {

    class null_fd
    {
    public:
        constexpr explicit null_fd() {}
        constexpr explicit null_fd(int) {}
        constexpr operator int() const { return -1; }
    };

    constexpr null_fd nullfd{};

    class auto_fd
    {
    private:
        int _fd;

    public:
        inline auto_fd(null_fd nfd = nullfd) noexcept : _fd {nfd} {}

        explicit inline auto_fd(int fd) noexcept : _fd {fd} {}

        inline auto_fd(auto_fd &&fd) noexcept : _fd {fd.release()} {}
        inline auto_fd &operator=(auto_fd &&fd) noexcept {
            reset(fd.release());
            return *this;
        }

        auto_fd(const auto_fd &) = delete;
        auto_fd &operator=(const auto_fd &) = delete;

        inline ~auto_fd() noexcept {
            reset();
        }

        inline int get() const noexcept { return _fd; }

        int release() noexcept
        {
            int r(_fd);
            _fd = -1;
            return r;
        }

        // Close an open file descriptor. Reset the descriptor to -1.
        inline void close() noexcept {
            if (_fd >= 0) {
                ::close(_fd);

                // If fdclose() failed then no reason to expect it to succeed the next time.
                _fd = -1;
            }
        }

        inline void reset(int fd = -1) noexcept {
            if (_fd >= 0) {
                close(); // Don't check for an error as not much we can do here.
            }

            _fd = fd;
        }

        inline bool valid() const {
            return _fd >= 0;
        }

    };

    inline bool operator==(const auto_fd &x, const auto_fd &y) {
        return x.get() == y.get();
    }

    inline bool operator!=(const auto_fd &x, const auto_fd &y) {
        return !(x == y);
    }

    inline bool operator==(const auto_fd &x, null_fd) {
        return x.get() == -1;
    }

    inline bool operator!=(const auto_fd &x, null_fd y) {
        return !(x == y);
    }

}
