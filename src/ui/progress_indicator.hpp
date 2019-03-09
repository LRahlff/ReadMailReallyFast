#pragma once

#include <memory>
#include <string>

namespace rmrf::ui {

/**
 * This class is used to share progress information on running tasks. 
 */
class progress_indicator {
public:
	/**
	 * Query the progress state
	 * @return The current progress in percent
	 */
	virtual int get_progress();
	/**
	 * This method shall be used in order to obtain
	 * the total amount of steps to be done.
	 */
	virtual int get_total_work();
	/**
	 * This method shall be used to retrieve the
	 * current progress as a number of finished jobs.
	 */
	virtual int get_current_work();
	/**
	 * This method shall be used in order to obtain the
	 * description of the total operation.
	 */
	virtual shared_ptr<string> get_operation_description();
	/**
	 * This method shall be used in order to retrieve a
	 * description of the current step.
	 */
	virtual shared_ptr<string> get_current_job_description();
}

}
