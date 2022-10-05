#include "ui/event.hpp"

namespace rmrf::ui {

event::event(const std::shared_ptr<ui_context> &sender) : event_sender(sender), handled(false) {
}

event::~event() {
}

std::shared_ptr<ui_context> event::get_sender() const {
    return this->event_sender;
}

bool event::has_been_handled() const {
    return this->handled;
}

void event::set_handled() {
    this->handled = true;
}

}
