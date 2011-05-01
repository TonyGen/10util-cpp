/** Thread helper functions */

#pragma once

#include <iostream>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <10util/mvar.h>
#include <10util/unit.h>

namespace thread {

typedef boost::shared_ptr <boost::thread> Thread;

/** Called when a thread exits abnormally. Initialized to default handler below. Assign your own handler if desired */
extern boost::function <void (std::exception&)> threadErrorHandler;

/** Just print error to stderr */
void defaultThreadErrorHandler (std::exception&);

}

namespace _thread { // private namespace

/** register this thread, run action, and unregister when finished. Report error on error.
 * A type: void A(), string A.toString() for thread description */
template <template <typename> class A> void runAction (A<void> action) {
	try {
		action();
	} catch (boost::thread_interrupted &e) {
		std::cout << boost::this_thread::get_id() << " interrupted." << std::endl;
	} catch (std::exception &e) {
		thread::threadErrorHandler (e);
	} catch (...) {
		std::cerr << "Uncaught non-std::exception in " << boost::this_thread::get_id() << std::endl;
	}
}

}

namespace thread {

/** Start new thread executing action.
 * A type: void A(), string A.toString() for thread description. */
template <template <typename> class A> Thread fork (A<void> action) {
	return boost::shared_ptr<boost::thread> (new boost::thread (boost::bind (_thread::runAction<A>, action)));
}

/** A parallel thread had an uncaught exception */
class FailedThread : public std::exception {
public:
	boost::thread::id threadId;
	std::string errorType;  // typically type name
	std::string errorMessage;
	FailedThread (boost::thread::id threadId, std::string errorType, std::string errorMessage) : threadId(threadId), errorType(errorType), errorMessage(errorMessage) {}
	FailedThread () {}  // for serialization
	~FailedThread () throw () {}
	const char* what() const throw () {  // override
		return ("(" + errorType + ") " + errorMessage) .c_str();
	}
};

}

namespace _thread {

/** Run action. On failure terminate given threads and set evar to current thread */
template <template <typename> class A> void runParAction (var::MVar_< std::vector<thread::Thread> > *vThreads, boost::shared_ptr<thread::FailedThread> *evar, A<void> action) {
	try {
		action ();
	} catch (std::exception &e) {
		evar->reset (new thread::FailedThread (boost::this_thread::get_id(), typeid(e).name(), e.what()));
		var::Access< std::vector<thread::Thread> > threads (*vThreads);
		for (unsigned i = 0; i < threads->size(); i++) (*threads)[i]->interrupt();
	}
}

}

namespace thread {

/** Fork each control action and continuous action and wait for all control actions to finish then terminate continuous actions. If any action fails then terminate the rest and reraise exception in main thread. Also, if main thread is interrupted then terminate all the threads. */
template <template <typename> class A> void parallel (std::vector< A<void> > controlActions, std::vector< A<void> > continuousActions) {
	// wrap threads in MVar so all threads are added before anyone fails and terminates them all, otherwise later ones would not be terminated because they started after failure.
	std::vector<Thread> _threads;
	var::MVar_< std::vector<Thread> > vThreads (_threads);
	boost::shared_ptr <FailedThread> evar;
	boost::function1< void, A<void> > run = boost::bind (_thread::runParAction<A>, &vThreads, &evar, _1);
	try {
		{
			var::Access< std::vector<Thread> > threads (vThreads);
			for (unsigned i = 0; i < controlActions.size(); i++) {
				boost::function0<void> f = boost::bind (run, controlActions[i]);
				threads->push_back (fork (f));
			}
			for (unsigned i = 0; i < continuousActions.size(); i++) {
				boost::function0<void> f = boost::bind (run, continuousActions[i]);
				threads->push_back (fork (f));
			}
		}
		std::vector<Thread> threads = vThreads.read();
		for (unsigned i = 0; i < controlActions.size(); i++)
			threads[i]->join();
		for (unsigned i = controlActions.size(); i < threads.size(); i++)
			threads[i]->interrupt();
		if (evar) throw *evar;
	} catch (boost::thread_interrupted &e) {
		{
			var::Access< std::vector<Thread> > threads (vThreads);
			for (unsigned i = 0; i < threads->size(); i++) (*threads)[i]->interrupt();
		}
		throw;
	}
}

inline void sleep (float secs) {boost::this_thread::sleep (boost::posix_time::milliseconds (secs * 1000));}

}
