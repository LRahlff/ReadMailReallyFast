#pragma once

#include <unistd.h>

namespace rmrf::net {

    /**
     * @class null_fd
     * @author leondietrich
     * @date 07/08/21
     * @file async_fd.hpp
     * @brief Null ptr file descriptor dummy class
     */
    class null_fd {
    public:
        constexpr explicit null_fd() {}
        constexpr explicit null_fd(int) {}
        constexpr operator int() const { return -1; }
    };

    /**
     * Instantiate a null FD.
     */
    constexpr null_fd nullfd{};

    /**
     * Automatic file descriptor.
     *
     * This class wraps raw file descriptors and handles their life time.
     *
     * @class auto_fd
     * @author leondietrich
     * @date 07/08/21
     * @file async_fd.hpp
     * @brief A file descriptor with smart (automatic) resource handling.
     */
    class auto_fd {
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

        /**
         * @brief This deconstructor frees (closes) the associated resource.
         */
        inline ~auto_fd() noexcept {
            reset();
        }

        /**
         * Use this method to obtain a raw file descriptor for usage with lower level APIs.
         * Calling this method won't invalidate the auto_fd. Don't keep this value around
         * (for immediate consumption only).
         * @brief Get the raw representation of the file descriptor for interaction with kernel APIs.
         * @return A raw file descriptor
         */
        inline int get() const noexcept { return _fd; }

        /**
         * Use this method in order to obtain a raw file descriptor from this auto_fd.
         * Calling this method will invalidate the auto_fd object. Deleting the auto_fd
         * after having called this method won't close the associated resource.
         * Try to avoid this method as handling raw file descriptors is way more error
         * prone than handling auto_fd.
         * @brief Get raw file descriptor for kernel APIs and cease control over it
         * @return The raw file descriptor
         */
        int release() noexcept {
            int r(_fd);
            _fd = -1;
            return r;
        }

        /**
         * Use this method in order to manually close the resource associated with this file descriptor.
         * After this method was executed this file descriptor is invalid. Further calls to this method
         * won't do anything.
         * @brief close the file descriptor
         */
        inline void close() noexcept {
            // Close an open file descriptor. Reset the descriptor to -1.
            if (_fd >= 0) {
                ::close(_fd);

                // If fdclose() failed then no reason to expect it to succeed the next time.
                _fd = -1;
            }
        }

        /**
         * Use this method in order to reset the file descriptor to a new one.
         * If the original file descriptor was in a valid state it will be closed.
         * @brief Reset the auto_fd object to the given raw file descriptor.
         * @param fd The new file descriptor to use.
         */
        inline void reset(int fd = -1) noexcept {
            if (_fd >= 0) {
                close(); // Don't check for an error as not much we can do here.
            }

            _fd = fd;
        }

        /**
         * Use this method in order to check if the file descriptor is still valid.
         * If the file descriptor has already been closed or is the nullfd this
         * method will return false.
         * @brief Check if the file descriptor is valid.
         * @return true if the file descriptor is still valid or otherwise false.
         */
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
