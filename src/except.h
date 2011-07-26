/* Throw exception but first print stack trace to stderr */

#pragma once

#include <stdexcept>
#include "stacktrace.h"
#include "type.h"
#include <iostream>

namespace except {

inline void raise (const std::exception &e) {
	std::cerr << typeName(e) << ": " << e.what() << std::endl;
	print_stacktrace ();
	throw e;
}

}
