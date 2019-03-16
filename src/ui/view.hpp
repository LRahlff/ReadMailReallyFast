#pragma once

#include <functional>
#include <memory>
#include <shared_mutex>

#include "ui/display.hpp"
#include "ui/event.hpp"
#include "ui/progress_indicator.hpp"
#include "ui/ui_context.hpp"

namespace rmrf::ui {

/**
 * This abstract class implements a view page.
 */
class view : public ui_context, private std::enable_shared_from_this<view> {
private:
    std::shared_ptr<view> parent_view;
    std::list<std::shared_ptr<view>> child_views;
private:
    void add_child(const std::shared_ptr<view> &child);
    void remove_child(const std::shared_ptr<view> &child);
public:
    /**
     * This method will be called when an operation is taking place. It may add
     * more indicators even when there are others still running due to multi tasking.
     *
     * @param progress The progress_indicator from the new running task to add
     */
    virtual void add_progress_indicator(const std::shared_ptr<progress_indicator> &progress);

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
    virtual bool update(const std::shared_ptr<display> &display, const std::shared_ptr<event> &event);
    /**
     * This method gets called when events need to be processed that do not
     * necessarily come from the UI thread.
     *
     * @param event The event that caused the update.
     */
    virtual void schedule_update(const std::shared_ptr<event> &event);
    /**
     * Use this method in order to retrieve the parent of this view.
     * @warn Keep in mind that this might be null.
     * @return The parent
     */
    std::shared_ptr<view> get_parent() const;
    /**
     * This constructor shall be capable of creating the view.
     *
     * @param parent The parent view of this view. This may be null if there is none.
     */
    explicit view(const std::shared_ptr<view> &parent);
    virtual ~view();
};

}
