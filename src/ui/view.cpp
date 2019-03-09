#include "lib/ncurses/ncurses.hpp"

#include "ui/view.hpp"

namespace rmrf::ui {

display::display() : m{} {
    initscr();
}

display::~display() {
    endwin();
}

}
