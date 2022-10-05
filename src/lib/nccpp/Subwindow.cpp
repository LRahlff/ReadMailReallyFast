#include "lib/nccpp/Subwindow.hpp"

#include <cassert>

#include "lib/nccpp/Window.hpp"

namespace nccpp {

inline Window &Subwindow::get_parent() {
    assert(win_ && "Invalid subwindow");

    return parent_;
}

inline int Subwindow::mvderwin(int y, int x) {
    assert(win_ && "Invalid subwindow");

    return ::mvderwin(win_, y, x);
}

inline void Subwindow::syncup() {
    assert(win_ && "Invalid subwindow");

    wsyncup(win_);
}

inline int Subwindow::syncok(bool on) {
    assert(win_ && "Invalid subwindow");

    return ::syncok(win_, on);
}

inline void Subwindow::cursyncup() {
    assert(win_ && "Invalid subwindow");

    wcursyncup(win_);
}

inline void Subwindow::syncdown() {
    assert(win_ && "Invalid subwindow");

    wsyncdown(win_);
}

inline void Subwindow::assign(WINDOW*) {
    assert(false && "Can't call nccpp::Subwindow::assign");
}

inline void Subwindow::destroy() {
    assert(false && "Can't call nccpp::Subwindow::destroy");
}

} // namespace nccpp
