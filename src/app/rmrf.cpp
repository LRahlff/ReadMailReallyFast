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

    // Start main loop of RMRF here
    h_nc->sync([](const display::ptr_type &) {
        mvprintw(0, 0, _("Starting RMRF…"));
    });

    return 0;
}
