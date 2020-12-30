#include "mumta/evloop.hpp"

#include <fcntl.h>

#include <functional>
#include <memory>

struct stdin_waiter;
struct stdin_waiter : std::enable_shared_from_this<stdin_waiter>
{
    ::ev::io e_stdin;

    stdin_waiter() : e_stdin{} {
        fcntl(0, F_SETFL, fcntl(0, F_GETFL)|O_NONBLOCK);
        e_stdin.set<stdin_waiter, &stdin_waiter::cb>(this);
        e_stdin.set(0, ::ev::READ);
        e_stdin.start();
    }
    ~stdin_waiter() {
        e_stdin.stop();
    }

    void cb(::ev::io &w, int events) {
        (void)w;
        (void)events;
        this->e_stdin.stop();
    }
};

void rmrf::ev::loop() {
    ::ev::default_loop defloop;

    auto w = std::make_shared<stdin_waiter>();

    defloop.run(0);
}
