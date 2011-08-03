/** Thread helper functions */

#pragma once

#include <iostream>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include "mvar.h"
#include "type.h"
#include "module.h"
#include "map.h"

namespace thread {

const module::Module module ("10util", "10util/thread.h");

class Thread {
	friend bool operator== (const Thread& a, const Thread& b) {return a.num == b.num && a.desc == b.desc;}
	friend bool operator< (const Thread& a, const Thread& b) {return a.num < b.num || (a.num == b.num && a.desc < b.desc);}
	friend bool operator!= (const Thread& a, const Thread& b) {return !(a == b);}
	friend bool operator> (const Thread& a, const Thread& b) {return b < a;}
	friend bool operator>= (const Thread& a, const Thread& b) {return !(a < b);}
	friend bool operator<= (const Thread& a, const Thread& b) {return !(a > b);}
public:
	unsigned num;
	std::string desc;
	Thread (unsigned num, std::string desc) : num(num), desc(desc) {}
	Thread () {}  // for serialization and dead thread
};

/** Called when a thread exits abnormally. Initialized to default handler below. Assign your own handler if desired */
extern boost::function <void (std::exception&)> threadErrorHandler;

/** Just print error to stderr */
void defaultThreadErrorHandler (std::exception&);

}

namespace _thread { // private namespace

/** Keep track of running threads */
class ThreadMap {
public:
	std::map <thread::Thread, boost::shared_ptr<boost::thread> > threadBoostMap;
	std::map <boost::thread::id, thread::Thread> idThreadMap;
	void addThread (thread::Thread t, boost::shared_ptr<boost::thread> p) {
		threadBoostMap [t] = p;
		idThreadMap [p->get_id()] = t;
	}
	void removeThread (boost::thread::id tid) {
		std::map <boost::thread::id, thread::Thread>::iterator it = idThreadMap.find (tid);
		thread::Thread t = it->second;
		std::map <thread::Thread, boost::shared_ptr<boost::thread> >::iterator it2 = threadBoostMap.find (t);
		idThreadMap.erase (it);
		threadBoostMap.erase (it2);
	}
	/** Empty (dead) Thread if thread already terminated */
	thread::Thread idThread (boost::thread::id tid) {
		return idThreadMap [tid];
	}
	/** Null ptr if thread already terminated */
	boost::shared_ptr<boost::thread> threadBoost (thread::Thread t) {
		return threadBoostMap [t];
	}
	std::vector<thread::Thread> allThreads () {
		return umap::values (idThreadMap);
	}
};
extern var::MVar_<ThreadMap> vThreadMap;

extern volatile unsigned nextThreadNum;

/** register this thread, run action, and unregister when finished. Report error on error.
 * A type: void A(), string A.toString() for thread description */
template <template <typename> class A> void runAction (A<void> action, var::MVar_<int>* syn) {
	syn->take(); // wait for this thread to be added to threadMap before running (and removing it)
	try {
		action();
	} catch (boost::thread_interrupted &e) {
		std::cout << boost::this_thread::get_id() << " interrupted." << std::endl;
	} catch (std::exception &e) {
		thread::threadErrorHandler (e);
	} catch (...) {
		std::cerr << "Uncaught non-std::exception in " << boost::this_thread::get_id() << std::endl;
	}
	var::Access<ThreadMap> threadMap (_thread::vThreadMap);
	threadMap->removeThread (boost::this_thread::get_id());
}

}

namespace thread {

/** Start new thread executing action */
template <template <typename> class A> Thread fork (A<void> action, std::string description) {
	var::MVar_<int> syn;
	boost::shared_ptr<boost::thread> p (new boost::thread (boost::bind (_thread::runAction<A>, action, &syn)));
	var::Access<_thread::ThreadMap> threadMap (_thread::vThreadMap);
	unsigned num = ++ _thread::nextThreadNum;
	Thread t (num, description);
	threadMap->addThread (t, p);
	syn.put(0); // thread can now run after we added it to threadMap
	return t;
}

/** Return all currently running threads */
inline std::vector<Thread> allThreads () {
	var::Access<_thread::ThreadMap> threadMap (_thread::vThreadMap);
	return threadMap->allThreads();
}

inline void interrupt (Thread t) {
	boost::shared_ptr<boost::thread> p;
	{	var::Access<_thread::ThreadMap> threadMap (_thread::vThreadMap);
		p = threadMap->threadBoost (t); }
	if (p) p->interrupt();
}

inline void join (Thread t) {
	boost::shared_ptr<boost::thread> p;
	{	var::Access<_thread::ThreadMap> threadMap (_thread::vThreadMap);
		p = threadMap->threadBoost (t); }
	if (p) p->join();
}

inline void sleep (double secs) {
	boost::this_thread::sleep (boost::posix_time::milliseconds (secs * 1000));
}

/** A parallel thread had an uncaught exception */
class FailedThread : public std::exception {
public:
	Thread thread;
	std::string errorType;  // typically type name
	std::string errorMessage;
	FailedThread (Thread thread, std::string errorType, std::string errorMessage) : thread(thread), errorType(errorType), errorMessage(errorMessage) {}
	FailedThread () {}  // for serialization
	~FailedThread () throw () {}
	const char* what() const throw () {  // override
		return ("(" + errorType + ") " + errorMessage + ": " + to_string (thread)) .c_str();
	}
};

}

namespace _thread {

/** Run action. On failure terminate given threads and set evar to current thread */
template <template <typename> class A> void runParAction (var::MVar_< std::vector<thread::Thread> >* vThreads, boost::shared_ptr<thread::FailedThread> *evar, A<void> action) {
	try {
		action ();
	} catch (std::exception &e) {
		{
			boost::thread::id tid = boost::this_thread::get_id();
			var::Access<ThreadMap> threadMap (vThreadMap);
			evar->reset (new thread::FailedThread (threadMap->idThread(tid), typeName(e), e.what()));
		}
		var::Access< std::vector<thread::Thread> > threads (*vThreads);
		for (unsigned i = 0; i < threads->size(); i++) thread::interrupt ((*threads)[i]);
	}
}

}

namespace thread {

/** Fork each control action and continuous action and wait for all control actions to finish then terminate continuous actions. If any action fails then terminate the rest and reraise exception in main thread. Also, if main thread is interrupted then terminate all the threads.
 * A is any function object (operator() method) that returns void */
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
				threads->push_back (fork (f, "parallel foreground"));
			}
			for (unsigned i = 0; i < continuousActions.size(); i++) {
				boost::function0<void> f = boost::bind (run, continuousActions[i]);
				threads->push_back (fork (f, "parallel background"));
			}
		}
		std::vector<Thread> threads = vThreads.read();
		for (unsigned i = 0; i < controlActions.size(); i++)
			join (threads[i]);
		for (unsigned i = controlActions.size(); i < threads.size(); i++)
			interrupt (threads[i]);
		if (evar) throw *evar;
	} catch (boost::thread_interrupted &e) {
		{
			var::Access< std::vector<Thread> > threads (vThreads);
			for (unsigned i = 0; i < threads->size(); i++) interrupt ((*threads)[i]);
		}
		throw;
	}
}

}

/* Printing & Serialization */

inline std::ostream& operator<< (std::ostream& out, const thread::Thread& p) {
	out << p.num << " " << p.desc; return out;}

inline std::ostream& operator<< (std::ostream& out, const thread::FailedThread& p) {
	out << p.what(); return out;}

namespace boost {namespace serialization {

template <class Archive> void serialize (Archive & ar, ::thread::Thread & x, const unsigned version) {
	ar & x.num;
	ar & x.desc;
}

template <class Archive> void serialize (Archive & ar, ::thread::FailedThread & x, const unsigned version) {
	ar & x.thread;
	ar & x.errorType;
	ar & x.errorMessage;
}

}}
