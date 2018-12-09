#pragma once

#include <functional>
#include <memory>
#include <shared_mutex>

namespace rmrf::ui {

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

}
