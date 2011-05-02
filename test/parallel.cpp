/* Test parallel threads
   Compile as:
     g++ parallel.cpp -o parallel -I/opt/local/include -L/opt/local/lib -lboost_system-mt -lboost_thread-mt -lboost_serialization-mt -l10util
   Run as:
     parallel */

#include <iostream>
#include <10util/thread.h>
#include <boost/bind.hpp>
#include <boost/function.hpp>

using namespace std;

void one () {
	thread::sleep (1);
	cout << "one" << endl;
	// throw runtime_error ("yo");
	thread::sleep (2);
	cout << "one done" << endl;
}

void two () {
	thread::sleep (2);
	cout << "two" << endl;
}

void three () {
	thread::sleep (3);
	cout << "three" << endl;
}

int main () {
	vector< boost::function0<void> > fore, aft;
	fore.push_back (boost::bind (one));
	fore.push_back (boost::bind (two));
	aft.push_back (boost::bind (three));
	try {
		thread::parallel (fore, aft);
	} catch (exception &e) {
		cerr << e.what() << endl;
	}
	cout << "all done" << endl;
}
