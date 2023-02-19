#pragma once

#include "lib/nccpp/Ncurses.hpp"
#include "lib/nccpp/Window.hpp"

namespace nccpp {

class Subwindow : public Window {
public:
    Subwindow(
        Window &parent,
        WINDOW* subwin,
        Window::Key /*dummy*/
    ) :
        Window{subwin},
        parent_{parent}
    {}

    Subwindow(const Subwindow &) = delete;
    Subwindow(Subwindow &&) = default;
    Subwindow &operator=(const Subwindow &) = delete;
    Subwindow &operator=(Subwindow &&) = default;

    ~Subwindow() = default;

    Window &get_parent();

    int mvderwin(int, int);

    void syncup();
    int syncok(bool);
    void cursyncup();
    void syncdown();

private:
    Window &parent_;

    void assign(WINDOW*) override;
    void destroy() override;
};

} // namespace nccpp
