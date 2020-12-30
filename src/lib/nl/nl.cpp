#include <lib/nl/nl.hpp>

#include <iomanip>
#include <sstream>
#include <string>

#include <netlink/utils.h>

#include "service/daemonctl.hpp"

bool check_version_libnl()
{
    auto nl_major{nl_ver_maj};
    auto nl_minor{nl_ver_min};

    constexpr auto exp_major{LIBNL_VER_MAJ};
    constexpr auto exp_minor{LIBNL_VER_MIN};

    std::stringstream str;
    str << "Checking dependency: libnl: detected " << std::dec << nl_major << "." << std::setw(2) << std::setfill('0') << nl_minor << ", compiled " << std::dec << exp_major << "." << std::setw(2) << std::setfill('0') << exp_minor;
    dctl_status_msg(str.str().c_str());

    if (nl_major != exp_major) {
        dctl_status_err("Checking dependency: libnl: failed version check: Major API version mismatch.\n");
        return false;
    }

    if (nl_minor < exp_minor) {
        dctl_status_err("Checking dependency: libnl: failed version check: Minor API version too old.\n");
        return false;
    }

    return true;
}
