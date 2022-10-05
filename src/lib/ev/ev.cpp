#include "lib/ev/ev.hpp"

#include <iomanip>
#include <sstream>
#include <string>

#include <ev++.h>

#include "service/daemonctl.hpp"

bool check_version_libev()
{
    auto ev_major{ev::version_major()};
    auto ev_minor{ev::version_minor()};

    constexpr auto exp_major{EV_VERSION_MAJOR};
    constexpr auto exp_minor{EV_VERSION_MINOR};

    std::stringstream str;
    str <<
        "Checking dependency: libev: detected " <<
        std::dec << ev_major << "." << std::setw(2) << std::setfill('0') << ev_minor <<
        ", compiled " <<
        std::dec << exp_major << "." << std::setw(2) << std::setfill('0') << exp_minor;
    dctl_status_msg(str.str().c_str());

    if (ev_major != exp_major) {
        dctl_status_err("Checking dependency: libev: failed version check: Major API version mismatch.\n");
        return false;
    }

    if (ev_minor < exp_minor) {
        dctl_status_err("Checking dependency: libev: failed version check: Minor API version too old.\n");
        return false;
    }

    return true;
}
