// Compile as: g++ mvar.cpp -g -o mvar -I/usr/local/include -L/usr/local/lib -lboost_system-mt -lboost_thread-mt -lboost_serialization-mt -l10util

#include <iostream>
#include <boost/thread.hpp>
#include <boost/smart_ptr.hpp>
#include <10util/mvar.h>

using namespace std;
using namespace boost;

static void one (MVAR(int) v) {
	var::Access<int> n (*v);
	cout << "one " << *n << endl;
	this_thread::sleep (boost::posix_time::seconds (5));
}

static void two (MVAR(int) v) {
	int n = v->read();
	cout << "two " << n << endl;
}

int main (int argc, const char* argv[]) {
	MVAR(int) v (new var::MVar_<int> (1));
	thread (boost::bind (one, v));
	thread (boost::bind (two, v));
	var::Access<int> n (*v);
	cout << "three " << *n << endl;
}
