#pragma once

#include <functional>
#include <memory>
#include <shared_mutex>

#include "progress_indicator.hpp"

namespace rmrf::ui {

/**
 * This class is designed to be the first level adapter to curses.
 * It implements some basic information handling and a registry for
 * views.
 */
class display : public std::enable_shared_from_this<display> {
public:
    typedef display self_type;
    typedef std::shared_ptr<self_type> ptr_type;

private:
    typedef std::shared_mutex mutex_type;
    typedef std::lock_guard<mutex_type> lock_type;
    mutable mutex_type m;

public:
    display();
    ~display();

public:
    template<typename F, typename ...Args>
    decltype(auto) sync(F &&f, Args &&... args) {
        lock_type lock(m);

        return std::forward<F>(f)(shared_from_this(), std::forward<Args>(args)...);
    }

};

/**
 * This abstract class implements a view page.
 */
virtual class view {
public:
    void set_progress_indicator(std::shared_ptr<progress_indicator>);
}

}
