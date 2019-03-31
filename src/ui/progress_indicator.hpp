#pragma once

#include <memory>
#include <string>

namespace rmrf::ui {

/**
 * This class is used to share progress information on running tasks.
 */
class progress_indicator {
public:
    progress_indicator() {};
    virtual ~progress_indicator() {};
    /**
     * Query the progress state
     * @return The current progress in percent
     */
    virtual int get_progress() const;
    /**
     * This method shall be used in order to obtain
     * the total amount of steps to be done.
     */
    virtual int get_total_work() const;
    /**
     * This method shall be used to retrieve the
     * current progress as a number of finished jobs.
     */
    virtual int get_current_work() const;
    /**
     * This method shall be used in order to obtain the
     * description of the total operation.
     */
    virtual std::shared_ptr<std::string> get_operation_description() const;
    /**
     * This method shall be used in order to retrieve a
     * description of the current step.
     */
    virtual std::shared_ptr<std::string> get_current_job_description() const;
};

}
