#include <ncurses/ncursesw.h>

#include "View.h"

namespace rmrf {
	namespace ui {

		void init_ui() {
			initscr();
		}

		void destroy_ui() {
			endwin();
		}

	}
}
