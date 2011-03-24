// Compile as: g++ test.cpp -o test -I/opt/local/include -L/opt/local/lib -lboost_system-mt -lboost_thread-mt -lboost_serialization-mt -l10util

#include <10util/util.h>
#include <cassert>
#include <iostream>

using namespace std;

int main (int argc, const char* argv[]) {
	vector<string> tokens = split_string (':', "hello d:world");
	assert (tokens.size() == 2);
	assert (tokens[0] == "hello d");
	assert (tokens[1] == "world");
	for (int i = 0; i < tokens.size(); i++)
		cout << tokens[i] << endl;

	tokens = split_string (':', "hello ");
	assert (tokens.size() == 1);
	assert (tokens[0] == "hello ");
	for (int i = 0; i < tokens.size(); i++)
		cout << tokens[i] << endl;

	tokens = split_string (':', ":hello ");
	assert (tokens.size() == 2);
	assert (tokens[0] == "");
	assert (tokens[1] == "hello ");
	for (int i = 0; i < tokens.size(); i++)
		cout << tokens[i] << endl;

	tokens = split_string (':', "hello :");
	assert (tokens.size() == 2);
	assert (tokens[0] == "hello ");
	assert (tokens[1] == "");
	for (int i = 0; i < tokens.size(); i++)
		cout << tokens[i] << endl;
}
