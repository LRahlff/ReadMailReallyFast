#include "lib/nccpp/Window.hpp"

#include <cassert>
#include <string>

namespace nccpp {

// getch

inline int Window::getch() {
    assert(win_ && "Window doesn't manage any object");

    return wgetch(win_);
}

inline int Window::mvgetch(int y, int x) {
    assert(win_ && "Window doesn't manage any object");

    return mvwgetch(win_, y, x);
}

// scanw

inline int Window::scanw(char const* fmt, ...) {
    assert(win_ && "Window doesn't manage any object");

    va_list args;
    va_start(args, fmt);
    auto ret = vw_scanw(win_, const_cast<char*>(fmt), args);
    va_end(args);

    return ret;
}

inline int Window::mvscanw(int y, int x, char const* fmt, ...) {
    assert(win_ && "Window doesn't manage any object");

    if ((this->move)(y, x) == ERR) {
        return ERR;
    }

    va_list args;
    va_start(args, fmt);
    auto ret = vw_scanw(win_, const_cast<char*>(fmt), args);
    va_end(args);

    return ret;
}

// getstr

inline int Window::getstr(std::string &str) {
    return (this->getnstr)(str, str.size());
}

inline int Window::getnstr(std::string &str, std::size_t n) {
    assert(win_ && "Window doesn't manage any object");

    str.resize(n);

    return wgetnstr(win_, &str[0], static_cast<int>(n));
}

inline int Window::mvgetstr(int y, int x, std::string &str) {
    return (this->mvgetnstr)(y, x, str, str.size());
}

inline int Window::mvgetnstr(int y, int x, std::string &str, std::size_t n) {
    return (this->move)(y, x) == ERR ? ERR : (this->getnstr)(str, n);
}

// inch

inline chtype Window::inch() {
    assert(win_ && "Window doesn't manage any object");

    return winch(win_);
}

inline chtype Window::mvinch(int y, int x) {
    assert(win_ && "Window doesn't manage any object");

    return mvwinch(win_, y, x);
}

// instr

inline int Window::instr(std::string &str) {
    return (this->innstr)(str, str.size());
}

inline int Window::innstr(std::string &str, std::size_t n) {
    assert(win_ && "Window doesn't manage any object");

    str.resize(n);

    return winnstr(win_, &str[0], static_cast<int>(n));
}

inline int Window::mvinstr(int y, int x, std::string &str) {
    return (this->mvinnstr)(y, x, str, str.size());
}

inline int Window::mvinnstr(int y, int x, std::string &str, std::size_t n) {
    return (this->move)(y, x) == ERR ? ERR : (this->innstr)(str, n);
}

// inchstr

inline int Window::inchstr(String &str) {
    return (this->inchnstr)(str, str.size());
}

inline int Window::inchnstr(String &str, std::size_t n) {
    assert(win_ && "Window doesn't manage any object");

    str.resize(n);

    return winchnstr(win_, &str[0], static_cast<int>(n));
}

inline int Window::mvinchstr(int y, int x, String &str) {
    return (this->mvinchnstr)(y, x, str, str.size());
}

inline int Window::mvinchnstr(int y, int x, String &str, std::size_t n) {
    return (this->move)(y, x) == ERR ? ERR : (this->inchnstr)(str, n);
}

} // namespace nccpp
