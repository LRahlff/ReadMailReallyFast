#pragma once

#include <ev++.h>

#if !EV_MULTIPLICITY
#error We require support for multiple event loops
#endif

namespace rmrf::ev {

bool init_libev();
bool init_watchdog();

void loop();
void stop();

}
