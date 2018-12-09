#ifdef __UNIX__
	#include <ncurses/cursesw.h>
#else
	#include <ncursesw/ncurses.h>
#endif

#include "ui/View.h"

namespace rmrf::ui {

display::display() {
    initscr();
}

display::~display() {
    endwin();
}

}
