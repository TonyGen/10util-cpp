
#include "thread.h"

template <> module::Module type<boost::thread>::module = module::Module ("boost_thread-mt", "boost/thread.hpp");

void thread::defaultThreadErrorHandler (std::exception &e) {
	std::cerr << "Uncaught exception in thread " << boost::this_thread::get_id() << ": (" << typeName(e) << ") " << e.what() << std::endl;
}

boost::function <void (std::exception&)> thread::threadErrorHandler = &thread::defaultThreadErrorHandler;

var::MVar_<_thread::ThreadMap> _thread::vThreadMap ((ThreadMap()));

volatile unsigned _thread::nextThreadNum = 0;
