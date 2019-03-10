#include <memory>

#include "ui/view.hpp"
#include "ui/new_child_event.hpp"

namespace rmrf::ui {

view::view(std::shared_ptr<view> parent) : parent_view{parent} {
	if(this->parent_view != nullptr) {
		std::shared_ptr<new_child_event> child_event(new new_child_event(this));
		this->parent_view->schedule_update(child_event);
		delete child_event;
	}
}

std::shared_ptr<view> view::get_parent() {
	return this->parent_view;
}

}
