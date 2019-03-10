#pragma once

#include <memory>
#include <string>

namespace rmrf::ui {

/**
 * This class is used to provide some basic infomation of
 * an UI object.
 */
class ui_context {
public:
	virtual ~ui_context();
	/**
	 * Use this method in order to get a fully qualified debug name
	 * of the context.
	 */
	virtual std::shared_ptr<std::string> get_name();
};

}
