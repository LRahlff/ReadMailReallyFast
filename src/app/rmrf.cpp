#include <libintl.h>
#include <locale.h>

#include <iostream>

#include "ui/view.hpp"

#define _(str) gettext(str)

int main() {
    using rmrf::ui::display;

    setlocale(LC_ALL, NULL);
    bindtextdomain("rmrf", "/usr/share/locale/");
    textdomain("rmrf");

    auto h_nc = std::make_shared<display>();

    h_nc->sync([](const display::ptr_type &) {
        std::cout << _("Hello World!\n") << std::flush;
    });

    return 0;
}
