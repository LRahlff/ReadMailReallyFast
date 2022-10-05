#include "lib/openssl/openssl.hpp"

#include <iomanip>
#include <sstream>
#include <string>

#include <openssl/crypto.h>

#include "service/daemonctl.hpp"

bool check_version_openssl()
{
    auto ossl_version{OpenSSL_version_num()};
    constexpr auto exp_version{OPENSSL_VERSION_NUMBER};
    constexpr auto exp_mask{0xFFFF0000UL};

    std::stringstream str;
    str <<
        "Checking dependency: libssl: detected " <<
        std::hex << std::setw(8) << ossl_version <<
        ", compiled " <<
        std::hex << std::setw(8) << ossl_version;
    dctl_status_msg(str.str().c_str());

    if ((ossl_version & exp_mask) != (exp_version & exp_mask)) {
        dctl_status_err("Checking dependency: libssl: failed version check: Major API version mismatch.\n");
        return false;
    }

    if ((ossl_version & ~exp_mask) < (exp_version & ~exp_mask)) {
        dctl_status_err("Checking dependency: libssl: failed version check: Minor API version too old.\n");
        return false;
    }

    return true;
}
