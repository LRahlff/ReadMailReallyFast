#include <iostream>

#include "lib/gettext/translations.hpp"

#include "lib/ncurses/ncurses.hpp"

#include "ui/view.hpp"

int main() {
    using rmrf::ui::display;

    setlocale(LC_ALL, "");
    bindtextdomain("rmrf", "/usr/share/locale");
    textdomain("rmrf");

    auto h_nc = std::make_shared<display>();

    h_nc->clear();

    for(size_t x = 0; x < 16; x++) {
        wint_t unichar{0};

        int ct = get_wch(&unichar); /* read character */
        h_nc->sync([x, ct, unichar](const display::ptr_type &) {
            mvprintw((int)x, 0, _("Detected key: %8x (ct=%i)"), unichar, ct);
        });
    }

    return 0;
}
