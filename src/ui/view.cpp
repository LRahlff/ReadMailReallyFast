#include <memory>

#include "ui/view.hpp"
#include "ui/new_child_event.hpp"

namespace rmrf::ui {

view::view(std::shared_ptr<view> parent) : parent_view{parent} {
	if(this->parent_view != nullptr) {
		this->parent_view->add_child(this);
	}
}

view::~view() {
	if(this->parent_view != nullptr) {
		this->parent_view->remove_child(this);
	}
	// Delete all childs that still exist
	this->child_views.clear();
}

std::shared_ptr<view> view::get_parent() {
	return this->parent_view;
}

void view::add_child(std::unique_ptr<view> child) {
	this->child_views.push_back(child);
}

void view::remove_child(std::unique_ptr<view> child) {
	this->child_views.remove(child);
}

}
