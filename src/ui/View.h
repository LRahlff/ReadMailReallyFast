#pragma once

#include <memory>

namespace rmrf::ui {

struct display : std::enable_shared_from_this<display> {
    display();
    ~display();
};

}
