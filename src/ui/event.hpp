#pragma once

#include <memory>
#include <string>
#include <list>

#include "ui/ui_context.hpp"

namespace rmrf::ui {

/**
 * This class is used to share publish event updates.
 */
class event {
private:
    std::shared_ptr<ui_context> event_sender;
    bool handled;

public:
    /**
     * In order to construct an event one needs at least to specify
     * the sender.
     *
     * @param sender The source of the event.
     */
    explicit event(const std::shared_ptr<ui_context> &sender);

    /**
     * A class implementing an event also needs to make sure that
     * the special payload of the event is beeing taken care of.
     */
    virtual ~event();

    /**
     * Use this methos in order to obtain the sender of this event.
     *
     * @return The sender
     */
    std::shared_ptr<ui_context> get_sender() const;

    /**
     * Use this function in order to get a human readable description
     * of the event.
     *
     * This does not need to be translated due to its purpose beeing
     * debugging.
     *
     * @return A pointer to the description string
     */
    std::shared_ptr<std::string> get_event_description() const;

    /**
     * Use this function in order to check if the event has been handled yet.
     *
     * @return true if the event was already dealt with or otherwise false.
     */
    bool has_been_handled() const;

    /**
     * Call this function once the purpose of the event has been taken care of.
     */
    void set_handled();
};

}
