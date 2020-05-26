#include <chrono>
#include <cstdint>
#include <thread>

#include "service/daemonctl.hpp"

int main() {
    dctl_status_msg("Initializing");
    dctl_status_msg("Reading configuration");
    dctl_status_msg("Initializing network");
    dctl_status_msg("Loading caches");
    dctl_status_msg("Refreshing caches");
    dctl_status_msg("Reading state");
    dctl_status_msg("Initializing");
    dctl_status_msg("Binding sockets");

    dctl_status_msg("Activating");
    dctl_status_ready();
    dctl_status_msg("Active");

    for(size_t x = 10; x; x--) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        dctl_watchdog_refresh();
    }

    dctl_status_msg("Preparing for shutdown");
    dctl_status_shutdown();
    dctl_status_msg("Finalizing pending transactions");
    dctl_status_msg("Storing active state");
    dctl_status_msg("Storing active caches");
    dctl_status_msg("Closing active sockets");
    dctl_status_msg("Inactive");
}
