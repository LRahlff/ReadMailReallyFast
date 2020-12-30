#include "service/daemonctl.hpp"

#ifdef __linux__

#include <systemd/sd-daemon.h>

#include <iostream>

ATTR_NONNULL_ALL
void dctl_status_msg(const char* msg) {
    sd_notifyf(0, "STATUS=%s", msg);
    std::cout << SD_INFO << "STATUS=" << msg << std::endl;
}

ATTR_NONNULL_ALL
void dctl_status_err(const char *msg) {
    sd_notifyf(0, "STATUS=%s", msg);
    std::cerr << SD_ERR << "STATUS=" << msg << std::endl;
}

void dctl_status_ready() {
    sd_notify(0, "READY=1");
}

void dctl_status_reload() {
    sd_notify(0, "RELOADING=1");
}

void dctl_status_shutdown() {
    sd_notify(0, "STOPPING=1");
}

void dctl_watchdog_refresh() {
    sd_notify(0, "WATCHDOG=1");
}

#endif
