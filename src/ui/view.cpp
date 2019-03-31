#include <memory>

#include "ui/view.hpp"
#include "ui/new_child_event.hpp"

namespace rmrf::ui {

view::view(const std::shared_ptr<view> &parent) : parent_view{parent}, child_views{} {
    if (this->parent_view) {
        this->parent_view->add_child(this->shared_from_this());
    }
}

view::~view() {
    // Delete all childs that still exist
    this->child_views.clear();

    // Notify our parent about us being destructed
    if (this->parent_view) {
        this->parent_view->remove_child(this->shared_from_this());
    }
}

std::shared_ptr<view> view::get_parent() const {
    return this->parent_view;
}

void view::add_child(const std::shared_ptr<view> &child) {
    this->child_views.push_back(child);
}

void view::remove_child(const std::shared_ptr<view> &child) {
    this->child_views.remove(child);
}

}
