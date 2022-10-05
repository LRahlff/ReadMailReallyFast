#include "lib/nccpp/Window.hpp"

#include <cassert>

#include "lib/ncurses/ncurses.hpp"

namespace nccpp {

inline int Window::move(int y, int x) {
    assert(win_ && "Window doesn't manage any object");

    return wmove(win_, y, x);
}

inline int Window::mvwin(int y, int x) {
    assert(win_ && "Window doesn't manage any object");

    return ::mvwin(win_, y, x);
}

inline int Window::erase() {
    assert(win_ && "Window doesn't manage any object");

    return werase(win_);
}

inline int Window::clear() {
    assert(win_ && "Window doesn't manage any object");

    return wclear(win_);
}

inline int Window::clrtobot() {
    assert(win_ && "Window doesn't manage any object");

    return wclrtobot(win_);
}

inline int Window::clrtoeol() {
    assert(win_ && "Window doesn't manage any object");

    return wclrtoeol(win_);
}

inline int Window::refresh() {
    assert(win_ && "Window doesn't manage any object");

    return wrefresh(win_);
}

inline int Window::outrefresh() {
    assert(win_ && "Window doesn't manage any object");

    return wnoutrefresh(win_);
}

inline int Window::redraw() {
    assert(win_ && "Window doesn't manage any object");

    return redrawwin(win_);
}

inline int Window::redrawln(int beg, int num) {
    assert(win_ && "Window doesn't manage any object");

    return wredrawln(win_, beg, num);
}

inline int Window::scroll(int n) {
    assert(win_ && "Window doesn't manage any object");

    return wscrl(win_, n);
}

inline void Window::get_yx(int &y, int &x) {
    assert(win_ && "Window doesn't manage any object");

    getyx(win_, y, x);
}

inline void Window::get_begyx(int &y, int &x) {
    assert(win_ && "Window doesn't manage any object");

    getbegyx(win_, y, x);
}

inline void Window::get_maxyx(int &y, int &x) {
    assert(win_ && "Window doesn't manage any object");

    getmaxyx(win_, y, x);
}

inline int Window::touchln(int start, int count, bool changed) {
    assert(win_ && "Window doesn't manage any object");

    return wtouchln(win_, start, count, changed);
}

inline bool Window::enclose(int y, int x) {
    assert(win_ && "Window doesn't manage any object");

    return wenclose(win_, y, x);
}

inline bool Window::coord_trafo(int &y, int &x, bool to_screen) {
    assert(win_ && "Window doesn't manage any object");

    return wmouse_trafo(win_, &y, &x, to_screen);
}

} // namespace nccpp
