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

void one (int n) {
	thread::sleep (n);
	cout << "one" << endl;
	// throw runtime_error ("yo");
}

void two (int n) {
	thread::sleep (n);
	cout << "two" << endl;
}

void three (int n) {
	thread::sleep (n);
	cout << "three" << endl;
}

int main () {
	vector< boost::function0<void> > fore, aft;
	fore.push_back (boost::bind (one, 1));
	fore.push_back (boost::bind (two, 2));
	aft.push_back (boost::bind (three, 3));
	try {
		thread::parallel (fore, aft);
	} catch (exception &e) {
		cerr << e.what() << endl;
	}
	cout << "done" << endl;
}
