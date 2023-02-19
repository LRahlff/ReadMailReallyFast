#include "lib/nccpp/Ncurses.hpp"

#include <algorithm>
#include <cassert>

#include "lib/nccpp/errors.hpp"

namespace nccpp {

inline Ncurses::Ncurses()    : Window{initscr()}, registered_colors_{},
#ifndef NDEBUG
    windows_ {}, is_exit_ {false},
#endif
    colors_initialized_ {false}
{
    if (!win_) {
        throw errors::NcursesInit{};
    }
}

inline Ncurses::~Ncurses() {
    endwin();
    win_ = nullptr;
#ifdef NO_LEAKS
    _nc_freeall();
#endif
}

#ifndef NDEBUG
inline void Ncurses::register_window_(Window &new_win, Window::Key /*dummy*/) {
    windows_.push_back(&new_win);
}

inline void Ncurses::unregister_window_(Window &win, Window::Key /*dummy*/) {
    auto it = std::find(std::begin(windows_), std::end(windows_), &win);

    assert(it != std::end(windows_));

    windows_.erase(it);
}
#endif

inline void Ncurses::exit_ncurses_mode() {
    assert(!is_exit_ && "Ncurses mode is already off");

#ifndef NDEBUG

    for (auto elem : windows_) {
        elem->invalidate_for_exit_(Window::Key{});
    }

    invalidate_for_exit_(Key{});
    is_exit_ = true;

#endif

    endwin();
}

inline void Ncurses::resume_ncurses_mode() {
    assert(is_exit_ && "Ncurses mode is already on");

#ifndef NDEBUG

    for (auto elem : windows_) {
        elem->validate_for_resume_(Window::Key{});
    }

    validate_for_resume_(Key{});
    is_exit_ = false;

#endif

    doupdate();
}

// Input options

inline int Ncurses::cbreak(bool on) {
    assert(!is_exit_ && "Ncurses mode is off");

    return on ? ::cbreak() : nocbreak();
}

inline int Ncurses::echo(bool on) {
    assert(!is_exit_ && "Ncurses mode is off");

    return on ? ::echo() : noecho();
}

inline int Ncurses::halfdelay(int delay) {
    assert(!is_exit_ && "Ncurses mode is off");

    return ::halfdelay(delay);
}

inline int Ncurses::intrflush(bool on) {
    assert(!is_exit_ && "Ncurses mode is off");

    return ::intrflush(win_, on);
}

inline int Ncurses::meta(bool on) {
    assert(!is_exit_ && "Ncurses mode is off");

    return ::meta(win_, on);
}

inline int Ncurses::raw(bool on) {
    assert(!is_exit_ && "Ncurses mode is off");

    return on ? ::raw() : noraw();
}

inline void Ncurses::qiflush(bool on) {
    assert(!is_exit_ && "Ncurses mode is off");

    on ? ::qiflush() : noqiflush();
}

inline int Ncurses::typeahead(int fd) {
    assert(!is_exit_ && "Ncurses mode is off");

    return ::typeahead(fd);
}

// Output options

inline int Ncurses::clearok(bool on, bool use_cs) {
    assert(!is_exit_ && "Ncurses mode is off");

    return::clearok(use_cs ? curscr : win_, on);
}

inline int Ncurses::idlok(bool on) {
    assert(!is_exit_ && "Ncurses mode is off");

    return ::idlok(win_, on);
}

inline void Ncurses::idcok(bool on) {
    assert(!is_exit_ && "Ncurses mode is off");

    ::idcok(win_, on);
}

inline void Ncurses::immedok(bool on) {
    assert(!is_exit_ && "Ncurses mode is off");

    ::immedok(win_, on);
}

inline int Ncurses::leaveok(bool on) {
    assert(!is_exit_ && "Ncurses mode is off");

    return ::leaveok(win_, on);
}

inline int Ncurses::scrollok(bool on) {
    assert(!is_exit_ && "Ncurses mode is off");

    return ::scrollok(win_, on);
}

inline int Ncurses::nl(bool on) {
    assert(!is_exit_ && "Ncurses mode is off");

    return on ? ::nl() : nonl();
}

// Input functions

inline int Ncurses::ungetch(int ch) {
    assert(!is_exit_ && "Ncurses mode is off");

    return ::ungetch(ch);
}

inline int Ncurses::has_key(int ch) {
    assert(!is_exit_ && "Ncurses mode is off");

    return ::has_key(ch);
}

// Misc

inline int Ncurses::doupdate() {
    assert(!is_exit_ && "Ncurses mode is off");

    return ::doupdate();
}

inline int Ncurses::line_count() {
    assert(!is_exit_ && "Ncurses mode is off");

    return LINES;
}

inline int Ncurses::column_count() {
    assert(!is_exit_ && "Ncurses mode is off");

    return COLS;
}

// Mouse

inline bool Ncurses::has_mouse() {
    assert(!is_exit_ && "Ncurses mode is off");

    return ::has_mouse();
}

inline int Ncurses::getmouse(MEVENT &event) {
    assert(!is_exit_ && "Ncurses mode is off");

    return ::getmouse(&event);
}

inline int Ncurses::ungetmouse(MEVENT &event) {
    assert(!is_exit_ && "Ncurses mode is off");

    return ::ungetmouse(&event);
}

inline mmask_t Ncurses::mousemask(mmask_t newmask, mmask_t* oldmask) {
    assert(!is_exit_ && "Ncurses mode is off");

    return ::mousemask(newmask, oldmask);
}

inline int Ncurses::mouseinterval(int erval) {
    assert(!is_exit_ && "Ncurses mode is off");

    return ::mouseinterval(erval);
}

// Window

inline WINDOW* Ncurses::newwin_(int nlines, int ncols, int begin_y, int begin_x, Window::Key /*dummy*/) {
    assert(!is_exit_ && "Ncurses mode is off");

    return newwin(nlines, ncols, begin_y, begin_x);
}

// Color

inline void Ncurses::start_color() {
    assert(!is_exit_ && "Ncurses mode is off");

    if (colors_initialized_) {
        return;
    }

    if (::start_color() == ERR) {
        throw errors::ColorInit{};
    }

    colors_initialized_ = true;
}

inline int Ncurses::use_default_colors() {
    assert(!is_exit_ && "Ncurses mode is off");

    start_color();

    return ::use_default_colors();
}

inline short Ncurses::color_to_pair_number(Color const &color) {
    assert(!is_exit_ && "Ncurses mode is off");

    auto it = std::find_if(
        std::begin(registered_colors_),
        std::end(registered_colors_),
        [color](Color const & elem) {
            return color == elem;
        });

    if (it != std::end(registered_colors_)) {
        return static_cast<short>(it - std::begin(registered_colors_) + 1);
    }

    start_color();

    // Ensure push_back will not throw
    registered_colors_.reserve(registered_colors_.size() + 1);
    auto res = init_pair(static_cast<short>(registered_colors_.size() + 1), color.foreground, color.background);

    if (res == ERR) {
        throw errors::TooMuchColors{color};
    }

    registered_colors_.push_back(color);

    return static_cast<short>(registered_colors_.size());
}

inline attr_t Ncurses::color_to_attr(Color const &color) {
    assert(!is_exit_ && "Ncurses mode is off");

    return static_cast<attr_t>(COLOR_PAIR(color_to_pair_number(color)));
}

inline Color Ncurses::pair_number_to_color(short pair_n) {
    assert(!is_exit_ && "Ncurses mode is off");
    assert(static_cast<std::size_t>(pair_n) <= registered_colors_.size() && "No such color");

    return registered_colors_[static_cast<std::size_t>(pair_n - 1)];
}

inline Color Ncurses::attr_to_color(attr_t a) {
    assert(!is_exit_ && "Ncurses mode is off");

    return pair_number_to_color(static_cast<short>(PAIR_NUMBER(static_cast<int>(a))));
}

inline int Ncurses::init_color(short color, short r, short g, short b) {
    assert(!is_exit_ && "Ncurses mode is off");

    start_color();

    return ::init_color(color, r, g, b);
}

inline void Ncurses::assign(WINDOW*) {
    assert(false && "Can't call nccpp::Ncurses::assign");
}

inline void Ncurses::destroy() {
    assert(false && "Can't call nccpp::Ncurses::destroy");
}

} // namespace nccpp
