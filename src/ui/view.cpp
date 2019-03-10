#include "ui/view.hpp"

namespace rmrf::ui {

view::view(std::shared_ptr<view> parent) {
	this->parent = parent;
}

std::shared_ptr<view> view::get_parent() {
	return this->parent;
}

}
