/*
 * netio_exception.cpp
 *
 *  Created on: 02.01.2021
 *      Author: doralitze
 */

#include "netio_exception.hpp"

namespace rmrf::net {

netio_exception::netio_exception(const std::string cause_) :
    cause(cause_)
{}

const char* netio_exception::what() const throw() {
    return this->cause.c_str();
}

}


