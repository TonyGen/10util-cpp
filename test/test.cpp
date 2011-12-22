// Compile as: g++ -I/usr/local/include test.cpp -o test -l10util

#include "../util.h"
//#include <10util/util.h>
//#include <10util/type.h>
#include <cassert>
#include <iostream>
#include <boost/function.hpp>
#include <10util/library.h>

using namespace std;

void test_split () {
	vector<string> tokens = split_string (':', "hello d:world");
	assert (tokens.size() == 2);
	assert (tokens[0] == "hello d");
	assert (tokens[1] == "world");

	tokens = split_string (':', "hello ");
	assert (tokens.size() == 1);
	assert (tokens[0] == "hello ");

	tokens = split_string (':', ":hello ");
	assert (tokens.size() == 2);
	assert (tokens[0] == "");
	assert (tokens[1] == "hello ");

	tokens = split_string (':', "hello :");
	assert (tokens.size() == 2);
	assert (tokens[0] == "hello ");
	assert (tokens[1] == "");
}

void test_parseInt () {
	int n = parse_string<int> ("42");
	assert (n == 42);
	try {
		parse_string<int> ("hello");
	} catch (std::runtime_error &e) {
		assert (string (e.what()) == string ("error parsing \"hello\" as type int"));
	}
}

struct Foo {};

int add (int x, int y) {return x + y;}

INITIALIZE (
	cout << "Hello, initializer " << add(2,3) << endl;
)

int main (int argc, const char* argv[]) {
	test_split();
	test_parseInt();
	cout << typeName<Foo>() << endl;
	cout << typeName< boost::function1<void,boost::function0<double> > >() << endl;
}
