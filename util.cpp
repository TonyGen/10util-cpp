/* */

#include "util.h"
#include <unistd.h>
#include <cerrno>

using namespace std;

Unit unit;

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

Pipe makePipe () {
	int pipeFds[2];
	int ok = pipe (pipeFds);
	if (ok == -1) throw std::runtime_error ("makePipe failed: " + to_string (errno));
	Pipe p;
	p.readEnd = pipeFds[0];
	p.writeEnd = pipeFds[1];
	return p;
}
