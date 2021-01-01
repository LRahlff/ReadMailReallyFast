#include "service/daemonctl.hpp"

#ifdef __FreeBSD__

ATTR_WEAK
void dctl_status_msg(const char* msg) {
    (void)msg;
}

ATTR_WEAK
void dctl_status_err(const char* msg) {
    (void)msg;
}

ATTR_WEAK
void dctl_status_ready() {

}

ATTR_WEAK
void dctl_status_reload() {

}

ATTR_WEAK
void dctl_status_shutdown() {

}

ATTR_WEAK
void dctl_watchdog_refresh() {

}

#endif
