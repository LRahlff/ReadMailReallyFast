#include "lib/nccpp/Window.hpp"

#include <cassert>

#include "lib/nccpp/errors.hpp"
#include "lib/nccpp/Ncurses.hpp"
#include "lib/nccpp/Subwindow.hpp"

namespace nccpp{

inline Window::Window(WINDOW* win) :
    win_{win},
#ifndef NDEBUG
    win_save_ {nullptr},
#endif
    subwindows_ {}
{
#ifndef NDEBUG

    if (win_ != stdscr) {
        ncurses().register_window_(*this, Key{});
    }

#endif
}

inline Window::Window(int nlines, int ncols, int begin_y, int begin_x) :
    win_{ncurses().newwin_(nlines, ncols, begin_y, begin_x, Key{})},
#ifndef NDEBUG
    win_save_ {nullptr},
#endif
    subwindows_ {}
{
    if (!win_) {
        throw errors::WindowInit{};
    }

#ifndef NDEBUG

    try {
        ncurses().register_window_(*this, Key{});
    }
    catch (...) {
        delwin(win_);
        throw;
    }

#endif
}

inline Window::Window(Window const &cp) :
    win_{nullptr},
#ifndef NDEBUG
    win_save_ {nullptr},
#endif
    subwindows_ {}
{
    assert(!cp.win_save_ && "Can't duplicate windows while ncurses mode is off");
    subwindows_.reserve(cp.subwindows_.size());

    if (cp.win_ && !(win_ = dupwin(cp.win_))) {
        throw errors::WindowInit{};
    }

#ifndef NDEBUG

    try {
        ncurses().register_window_(*this, Key{});
    }
    catch (...) {
        delwin(win_);
        throw;
    }

#endif
}

inline Window &Window::operator=(Window const &cp){
    if (this != &cp) {
        Window tmp{cp};
        *this = std::move(tmp);
    }

    return *this;
}

inline Window::Window(Window &&mv)
#ifdef NDEBUG
noexcept
#endif
    :
    win_ {mv.win_},
#ifndef NDEBUG
    win_save_ {mv.win_save_},
#endif
    subwindows_ {std::move(mv.subwindows_)}
{
    mv.win_ = nullptr;
#ifndef NDEBUG
    mv.win_save_ = nullptr;
    ncurses().register_window_(*this, Key{});
#endif
}

inline Window &Window::operator=(Window &&mv) noexcept {
    if (this != &mv) {
        destroy();
        win_ = mv.win_;
        mv.win_ = nullptr;

#ifndef NDEBUG
        win_save_ = mv.win_save_;
        mv.win_save_ = nullptr;
#endif

        subwindows_ = std::move(mv.subwindows_);
    }

    return *this;
}

inline Window::~Window() {
#ifndef NDEBUG

    if (this != &ncurses()) {
        ncurses().unregister_window_(*this, Key{});
    }

#endif

    destroy();
}

inline void Window::assign(WINDOW* new_win) {
    assert(!win_save_ && "Can't modify window while ncurses mode is off");

    if (win_) {
        destroy();
    }

    win_ = new_win;
}

inline void Window::destroy() {
    if (win_) {
        subwindows_.clear();
        delwin(win_);
        win_ = nullptr;
    }

#ifndef NDEBUG
    else if (win_save_) {
        subwindows_.clear();
        delwin(win_save_);
        win_save_ = nullptr;
    }

#endif
}

inline WINDOW* Window::get_handle() {
    assert(win_ && "Window doesn't manage any object");

    return win_;
}

inline WINDOW const* Window::get_handle() const {
    assert(win_ && "Window doesn't manage any object");
    return win_;
}

inline std::size_t Window::add_subwindow(int lines, int cols, int beg_y, int beg_x) {
    assert(win_ && "Window doesn't manage any object");

#ifndef NDEBUG
    int posx = 0, posy = 0, maxx = 0, maxy = 0;
    get_begyx(posy, posx);
    get_maxyx(maxy, maxx);
    maxy += posy;
    maxx += posx;
#endif

    assert(posy <= beg_y && posx <= beg_x && maxy >= beg_y + lines && maxx >= beg_x + cols &&
           "Invalid subwindow coordinates");

    auto new_subw = subwin(win_, lines, cols, beg_y, beg_x);

    if (!new_subw) {
        throw errors::WindowInit{};
    }

    try {
        subwindows_.emplace_back(*this, new_subw, Window::Key{});
    }
    catch (...) {
        delwin(new_subw);
        throw;
    }

    return subwindows_.size() - 1;
}

inline Subwindow &Window::get_subwindow(std::size_t index) {
    assert(win_ && "Window doesn't manage any object");
    assert(index < subwindows_.size() && subwindows_[index].win_ && "Invalid subwindow");

    return subwindows_[index];
}

inline void Window::delete_subwindow(std::size_t index) {
    assert(win_ && "Window doesn't manage any object");
    assert(index < subwindows_.size() && subwindows_[index].win_ && "Invalid subwindow");

    subwindows_[index].~Subwindow();
    new (&subwindows_[index]) Subwindow{*this, nullptr, Key{}};
}

#ifndef NDEBUG
inline void Window::invalidate_for_exit_(Window::Key /*dummy*/) {
    for (auto &elem : subwindows_) {
        elem.invalidate_for_exit_(Key{});
    }

    win_save_ = win_;
    win_ = nullptr;
}

inline void Window::validate_for_resume_(Window::Key /*dummy*/) {
    for (auto &elem : subwindows_) {
        elem.validate_for_resume_(Key{});
    }

    win_ = win_save_;
    win_save_ = nullptr;
}
#endif

inline int overlay(Window const &src, Window &dst) {
    return ::overlay(src.get_handle(), dst.get_handle());
}

inline int overwrite(Window const &src, Window &dst) {
    return ::overwrite(src.get_handle(), dst.get_handle());
}

inline int copywin(
    Window const &src,
    Window &dst,
    int sminrow,
    int smincol,
    int dminrow,
    int dmincol,
    int dmaxrow,
    int dmaxcol,
    bool overlay
) {
    return ::copywin(
               src.get_handle(),
               dst.get_handle(),
               sminrow,
               smincol,
               dminrow,
               dmincol,
               dmaxrow,
               dmaxcol,
               overlay);
}

} // namespace nccpp
