#include "lib/nccpp/Window.hpp"

#include <cassert>

namespace nccpp {

// addch

inline int Window::addch(chtype const ch) {
    assert(win_ && "Window doesn't manage any object");

    return waddch(win_, ch);
}

inline int Window::mvaddch(int y, int x, chtype const ch) {
    assert(win_ && "Window doesn't manage any object");

    return mvwaddch(win_, y, x, ch);
}

inline int Window::echochar(chtype const ch) {
    assert(win_ && "Window doesn't manage any object");

    return wechochar(win_, ch);
}

// printw

inline int Window::printw(char const* fmt, ...) {
    assert(win_ && "Window doesn't manage any object");

    va_list args;
    va_start(args, fmt);
    auto ret = vw_printw(win_, fmt, args);
    va_end(args);

    return ret;
}

inline int Window::mvprintw(int y, int x, char const* fmt, ...) {
    assert(win_ && "Window doesn't manage any object");

    if ((this->move)(y, x) == ERR) {
        return ERR;
    }

    va_list args;
    va_start(args, fmt);
    auto ret = vw_printw(win_, fmt, args);
    va_end(args);

    return ret;
}

// addstr

inline int Window::addstr(std::string const &str) {
    return (this->addnstr)(str, str.size());
}

inline int Window::addnstr(std::string const &str, std::size_t n) {
    assert(win_ && "Window doesn't manage any object");
    assert(n <= str.size());

    return waddnstr(win_, str.c_str(), static_cast<int>(n));
}

inline int Window::mvaddstr(int y, int x, std::string const &str) {
    return (this->mvaddnstr)(y, x, str, str.size());
}

inline int Window::mvaddnstr(int y, int x, std::string const &str, std::size_t n) {
    return (this->move)(y, x) == ERR ? ERR : (this->addnstr)(str, n);
}

// addchstr

inline int Window::addchstr(String const &chstr) {
    return (this->addchnstr)(chstr, chstr.size());
}

inline int Window::addchnstr(String const &chstr, std::size_t n) {
    assert(win_ && "Window doesn't manage any object");
    assert(n <= chstr.size());

    return waddchnstr(win_, chstr.c_str(), static_cast<int>(n));
}

inline int Window::mvaddchstr(int y, int x, String const &chstr) {
    return (this->mvaddchnstr)(y, x, chstr.c_str(), chstr.size());
}

inline int Window::mvaddchnstr(int y, int x, String const &chstr, std::size_t n) {
    return (this->move)(y, x) ? ERR : (this->addchnstr)(chstr, n);
}

// insch

inline int Window::insch(chtype ch) {
    assert(win_ && "Window doesn't manage any object");

    return winsch(win_, ch);
}

inline int Window::mvinsch(int y, int x, chtype ch) {
    assert(win_ && "Window doesn't manage any object");

    return mvwinsch(win_, y, x, ch);
}

// insstr

inline int Window::insstr(std::string const &str) {
    return (this->insnstr)(str, str.size());
}

inline int Window::insnstr(std::string const &str, std::size_t n) {
    assert(win_ && "Window doesn't manage any object");
    assert(n <= str.size());

    return winsnstr(win_, str.c_str(), static_cast<int>(n));
}

inline int Window::mvinsstr(int y, int x, std::string const &str) {
    return (this->mvinsnstr)(y, x, str, str.size());
}

inline int Window::mvinsnstr(int y, int x, std::string const &str, std::size_t n) {
    return (this->move)(y, x) == ERR ? ERR : (this->insnstr)(str, n);
}

// delch

inline int Window::delch() {
    assert(win_ && "Window doesn't manage any object");

    return wdelch(win_);
}

inline int Window::mvdelch(int y, int x) {
    assert(win_ && "Window doesn't manage any object");

    return mvwdelch(win_, y, x);
}

// deleteln

inline int Window::insdelln(int n) {
    assert(win_ && "Window doesn't manage any object");

    return winsdelln(win_, n);
}

} // namespace nccpp
