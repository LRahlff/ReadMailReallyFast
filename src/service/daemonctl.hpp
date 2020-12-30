#pragma once

#include "macros.hpp"

ATTR_NONNULL_ALL
void dctl_status_msg(const char* msg);

ATTR_NONNULL_ALL
void dctl_status_err(const char *msg);

void dctl_status_ready();
void dctl_status_reload();
void dctl_status_shutdown();

void dctl_watchdog_refresh();
