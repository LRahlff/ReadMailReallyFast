#include "lib/nccpp/Window.hpp"

#include <cassert>

#include "lib/nccpp/Color.hpp"
#include "lib/nccpp/Ncurses.hpp"

namespace nccpp {

inline int Window::attroff(int a) {
    assert(win_ && "Window doesn't manage any object");

    return wattroff(win_, a);
}

inline int Window::attron(int a) {
    assert(win_ && "Window doesn't manage any object");

    return wattron(win_, a);
}

inline int Window::attrset(int a) {
    assert(win_ && "Window doesn't manage any object");

    return wattrset(win_, a);
}

inline int Window::attr_get(attr_t &a) {
    assert(win_ && "Window doesn't manage any object");

    return wattr_get(win_, &a, nullptr, nullptr);
}

inline int Window::color_get(Color &c) {
    assert(win_ && "Window doesn't manage any object");

    short pair_n{0};

    if (wattr_get(win_, nullptr, &pair_n, nullptr) == ERR) {
        return ERR;
    }

    c = nccpp::ncurses().pair_number_to_color(pair_n);

    return OK;
}

inline int Window::attr_color_get(attr_t &a, Color &c) {
    assert(win_ && "Window doesn't manage any object");

    short pair_n{0};

    if (wattr_get(win_, &a, &pair_n, nullptr) == ERR) {
        return ERR;
    }

    c = nccpp::ncurses().pair_number_to_color(pair_n);

    return OK;
}

inline int Window::chgat(int n, attr_t a, Color c) {
    assert(win_ && "Window doesn't manage any object");

    return ::wchgat(win_, n, a, nccpp::ncurses().color_to_pair_number(c), nullptr);
}

inline int Window::mvchgat(int y, int x, int n, attr_t a, Color c) {
    assert(win_ && "Window doesn't manage any object");

    return (this->move)(y, x) == ERR ? ERR : (this->chgat)(n, a, c);
}

} // namespace nccpp
