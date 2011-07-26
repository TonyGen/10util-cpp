/* */

#include "util.h"
#include <unistd.h>
#include <cerrno>

using namespace std;

/** Tokenize string using given char as delimeter */
vector<string> split_string (char delimeter, string str) {
	vector<string> tokens;
	string::size_type i = 0;
	string::size_type j = 0;
	while ((j = str.find (delimeter, j)) != string::npos) {
		tokens.push_back (str.substr (i, j-i));
		i = ++j;
	}
	tokens.push_back (str.substr (i, str.length() - i));
	return tokens;
}
