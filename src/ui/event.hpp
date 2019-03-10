#pragma once

#include <memory>
#include <string>

#include "ui_context.hpp"

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
	event(std::shared_ptr<ui_context> sender);
	/**
	 * A class implementing an event also needs to make sure that
	 * the special payload of the event is beeing taken care of.
	 */
	~event();
	/**
	 * Use this methos in order to obtain the sender of this event.
	 *
	 * @return The sender
	 */
	std::shared_ptr<ui_context> get_sender();
	/**
	 * Use this function in order to get a human readable description
	 * of the event.
	 *
	 * This does not need to be translated due to its purpose beeing
	 * debugging.
	 *
	 * @return A pointer to the description string
	 */
	std::shared_ptr<std::string> get_event_description();
	/**
	 * Use this function in order to check if the event has been handled yet.
	 *
	 * @return true if the event was already dealt with or otherwise false.
	 */
	bool has_been_handled();
	/**
	 * Call this function once the purpose of the event has been taken care of.
	 */
	void set_handled();
};

}
