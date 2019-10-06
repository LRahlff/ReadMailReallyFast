#include "ui/display.hpp"

#include "lib/ncurses/ncurses.hpp"

#include "ui/view.hpp"

namespace rmrf::ui {

display::display() : m{} {
    initscr();
    raw();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
}

display::~display() {
    endwin();
}

void display::clear() {
    this->sync([](const display::ptr_type &) {
        ::clear();
    });
}

}
