#include <iostream>

#include "lib/gettext/translations.hpp"

#include "lib/ncurses/ncurses.hpp"

#include "lib/nccpp/ncursescpp.hpp"

#include "ui/view.hpp"

int main() {
    using rmrf::ui::display;

    setlocale(LC_ALL, "");
    bindtextdomain("rmrf", "/usr/share/locale");
    textdomain("rmrf");

    auto h_nc = std::make_shared<display>();

    h_nc->clear();

    bool loop = true;

    while(loop) {
        wint_t unichar{0};

        int ct = getch(); //(&unichar); /* read character */
        h_nc->sync([ct, unichar](const display::ptr_type &) {
            mvprintw(0, 0, _("Detected key: %8x (ct=%i)"), unichar, ct);
        });

        if(10 == ct) {
            loop = false;
        }
    }

    return 0;
}
