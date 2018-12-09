#include <iostream>

#include "ui/view.hpp"

int main() {
    using rmrf::ui::display;

    auto h_nc = std::make_shared<display>();

    h_nc->sync([](const display::ptr_type &) {
        std::cout << "Hello World!\n" << std::flush;
    });

    return 0;
}
