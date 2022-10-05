#include "lib/nccpp/Window.hpp"

#include <cassert>

namespace nccpp {

inline int Window::keypad(bool on) {
    assert(win_ && "Window doesn't manage any object");

    return ::keypad(win_, on);
}

inline int Window::nodelay(bool on) {
    assert(win_ && "Window doesn't manage any object");

    return ::nodelay(win_, on);
}

inline int Window::notimeout(bool on) {
    assert(win_ && "Window doesn't manage any object");

    return ::notimeout(win_, on);
}

inline void Window::timeout(int delay) {
    assert(win_ && "Window doesn't manage any object");

    wtimeout(win_, delay);
}

inline int Window::clearok(bool on) {
    assert(win_ && "Window doesn't manage any object");

    return ::clearok(win_, on);
}

inline int Window::setscrreg(int top, int bot) {
    assert(win_ && "Window doesn't manage any object");

    return wsetscrreg(win_, top, bot);
}

} // namespace nccpp
