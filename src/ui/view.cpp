#ifdef __UNIX__
	#include <ncurses/cursesw.h>
#else
	#include <ncursesw/ncurses.h>
#endif

#include "ui/view.hpp"

namespace rmrf::ui {

display::display() : m{} {
    initscr();
}

display::~display() {
    endwin();
}

}
