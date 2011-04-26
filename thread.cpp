
#include "thread.h"

void thread::defaultThreadErrorHandler (std::exception &e) {
	std::cerr << "Uncaught exception in thread " << boost::this_thread::get_id() << ": (" << typeid(e).name() << ") " << e.what() << std::endl;
}

boost::function <void (std::exception&)> thread::threadErrorHandler = &thread::defaultThreadErrorHandler;
