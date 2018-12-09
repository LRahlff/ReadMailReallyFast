#include <ncursesw/ncurses.h>

#include "ui/View.h"

namespace rmrf::ui {

display::display() {
    initscr();
}

display::~display() {
    endwin();
}

}
