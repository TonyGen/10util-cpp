// Compile as: g++ test.cpp -o test -I/opt/local/include -L/opt/local/lib -lboost_system-mt -lboost_thread-mt -lboost_serialization-mt -l10util

#include <10util/util.h>
#include <10util/message.h>
#include <cassert>
#include <iostream>

using namespace std;

void test_split () {
	vector<string> tokens = split_string (':', "hello d:world");
	assert (tokens.size() == 2);
	assert (tokens[0] == "hello d");
	assert (tokens[1] == "world");
	for (unsigned i = 0; i < tokens.size(); i++)
		cout << tokens[i] << endl;

	tokens = split_string (':', "hello ");
	assert (tokens.size() == 1);
	assert (tokens[0] == "hello ");
	for (unsigned i = 0; i < tokens.size(); i++)
		cout << tokens[i] << endl;

	tokens = split_string (':', ":hello ");
	assert (tokens.size() == 2);
	assert (tokens[0] == "");
	assert (tokens[1] == "hello ");
	for (unsigned i = 0; i < tokens.size(); i++)
		cout << tokens[i] << endl;

	tokens = split_string (':', "hello :");
	assert (tokens.size() == 2);
	assert (tokens[0] == "hello ");
	assert (tokens[1] == "");
	for (unsigned i = 0; i < tokens.size(); i++)
		cout << tokens[i] << endl;
}

void test_intBytes () {
	unsigned int n = _message::bytesAsInt (_message::intAsBytes (228));
	std::cout << n << std::endl;
	assert (n == 228);
}

void test_parseInt () {
	int n = parse_string<int> ("42");
	assert (n == 42);
	try {
		parse_string<int> ("hello");
	} catch (std::runtime_error &e) {
		assert (std::string (e.what()) == "error parsing \"hello\" as type i");
	}
}

int main (int argc, const char* argv[]) {
	test_split();
	test_intBytes();
	test_parseInt();
}
