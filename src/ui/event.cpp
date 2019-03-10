#include "ui/event.hpp"

namespace rmrf::ui {
	event::event(std::shared_ptr<ui_context> sender) {
		this->event_sender = sender;
	}

	event::~event() {
		this->event_sender = nullptr;
		this->handled = false;
	}

	std::shared_ptr<ui_context> event::get_sender() {
		return this->event_sender;
	}

	bool event::has_been_handled() {
		return this->handled;
	}

	void event::set_handled() {
		this->handled = true;
	}
}
