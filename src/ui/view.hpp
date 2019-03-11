#pragma once

#include <functional>
#include <memory>
#include <shared_mutex>

#include "progress_indicator.hpp"
#include "ui_context.hpp"
#include "event.hpp"

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
class view : public ui_context {
private:
    std::shared_ptr<view> parent_view;
public:
    /**
     * This method will be called when an operation is taking place. It may add
     * more indicators even when there are others still running due to multi tasking.
     *
     * @param progress The progress_indicator from the new running task to add
     */
    virtual void add_progress_indicator(std::shared_ptr<progress_indicator> progress);

    /**
     * This method will be called on a regular basis when the view needs to be updated
     * due to certain events.
     *
     * A view is required to also invoke the update methos on its clients.
     *
     * @param display The display to update the view on.
     * @param event The event that caused the update.
     * @return True if rerendering is required or otherwise false.
     */
    virtual bool update(std::shared_ptr<display> display, std::shared_ptr<event> event);
    /**
     * This method gets called when events need to be processed that do not
     * necessarily come from the UI thread.
     *
     * @param event The event that caused the update.
     */
    virtual void schedule_update(std::shared_ptr<event> event);
    /**
     * Use this method in order to retrieve the parent of this view.
     * @warn Keep in mind that this might be null.
     * @return The parent
     */
    std::shared_ptr<view> get_parent();
    /**
     * This constructor shall be capable of creating the view.
     *
     * @param parent The parent view of this view. This may be null if there is none.
     */
    view(std::shared_ptr<view> parent);
    virtual ~view();
};

}
