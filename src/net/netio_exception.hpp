/*
 * netio_exception.hpp
 *
 *  Created on: 02.01.2021
 *      Author: doralitze
 */

#pragma once


#include <exception>
#include <string>

namespace rmrf::net {

class netio_exception : public std::exception {
private:
    std::string cause;

public:
    netio_exception(const std::string cause_);
    virtual const char* what() const throw();
};

}
