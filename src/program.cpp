/* */

#include "program.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <csignal>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include "type.h"
#include "module.h"
#include "util.h" // md5
#include <boost/algorithm/string.hpp>

using namespace std;

template <> module::Module type<program::Program>::module = module::Module ("10util", "10util/program.h");

/** Short hand for options list of one pair */
program::Options program::options (string name, string value) {
	Options opts;
	opts.push_back (make_pair (name, value));
	return opts;
}
/** Short hand for options list of two pairs */
program::Options program::options (string name1, string value1, string name2, string value2) {
	Options opts;
	opts.push_back (make_pair (name1, value1));
	opts.push_back (make_pair (name2, value2));
	return opts;
}
/** Short hand for options list of three pairs */
program::Options program::options (string name1, string value1, string name2, string value2, string name3, string value3) {
	Options opts;
	opts.push_back (make_pair (name1, value1));
	opts.push_back (make_pair (name2, value2));
	opts.push_back (make_pair (name3, value3));
	return opts;
}

/** Merge first program options into second, i.e. second arg has precedence */
program::Options program::merge (Options secondary, Options primary) {
	Options result;
	Options diff = secondary;  // copy vector
	for (unsigned i = 0; i < primary.size(); i++) {
		unsigned resultSize = result.size();
		for (Options::iterator j = diff.begin(); j != diff.end(); ++j) {
			if (j->first == primary[i].first) {
				result.push_back (*j);
				diff.erase (j);
				break;
			}
		}
		if (result.size() == resultSize) result.push_back (primary[i]);
	}
	for (unsigned j = 0; j < diff.size(); j ++)
		result.push_back (diff[j]);
	return result;
}

/** Return value of key, or null ptr if missing */
boost::optional <string> program::lookup (string key, Options options) {
	for (unsigned i = 0; i < options.size(); i ++) {
		if (options[i].first == key)
			return options[i].second;
	}
	return boost::none;
}

string program::optionsString (const Options &programOptions) {
	stringstream ss;
	for (unsigned i = 0; i < programOptions.size(); i++) {
		string key = programOptions[i].first;
		if (!key.empty() && key[0] != '-') key = (key.size() == 1 ? "-" : "--") + key;
		ss << key << " " << programOptions[i].second << " ";
	}
	return ss.str();
}

/** Parse cmd-line args into options association list. Note: a direct arg following a no-arg option will be interpreted as the arg of the option. This is tolerable since unparsing (optionString) will reproduce the same line, but it will fool lookup and merge, so don't use these in this case. */
program::Options program::parseArgs (vector<string> args) {
	program::Options options;
	boost::optional<string> prevKey = boost::none; // String = the key seen last, Null = a value was seen last
	for (vector<string>::iterator arg = args.begin(); arg != args.end(); ++arg)
		if ((*arg)[0] == '-') {
			if (prevKey) options.push_back (make_pair (*prevKey, ""));
			boost::trim_left_if (*arg, boost::is_any_of ("-"));
			prevKey = *arg;
		} else {
			options.push_back (make_pair (prevKey ? *prevKey : "", *arg));
			prevKey = boost::none;
		}
	if (prevKey) options.push_back (make_pair (*prevKey, ""));
	return options;
}

/** Filename of executable excluding path */
string program::Program::executableName () const {
	vector<string> parts;
	boost::split (parts, executable, boost::is_any_of ("/"));
	return *--parts.end();
}

/** MD5 hex string derived (using md5) from program name its options, so each program will have a different but reproducible hex string. */
string program::md5Name (Program p) {return p.executableName() + md5 (p.command());}

typedef char* cString;

static char* cStringCopy (string s) {
	char* copy = new char [s.size() + 1];
	strcpy (copy, s.c_str());
	return copy;
}

/** Convert program + options to list of arguments for 'execvp'.
 * First arg must be executable, and last arg must be NULL */
static cString* argv (program::Program program) {
	cString* args = new cString [program.options.size() * 2 + 2];
	unsigned j = 0;
	args[j++] = cStringCopy (program.executable);
	for (unsigned i = 0; i < program.options.size(); i++) {
		string key = program.options[i].first;
		if (!key.empty() && key[0] != '-') key = (key.size() == 1 ? "-" : "--") + key;
		string val = program.options[i].second;
		if (!key.empty()) args[j++] = cStringCopy (key);
		if (!val.empty()) args[j++] = cStringCopy (val);
	}
	args[j] = NULL;
	return args;
}

/** Run command and wait for termination. Fail if command fails */
static void runCommand (string command) {
	int exitStatus = system (command.c_str());
	if (exitStatus != 0) {
		stringstream ss;
		ss << "Command failed with exit " << exitStatus << ": " << command;
		throw runtime_error (ss.str());
	}
}

/** Redirect std/in/out/err to files named, if present */
/*static void redirectFd (string filename, int openFlags, int openMode, int dupFd) {
	if (! filename.empty()) {
		int fd = open (filename.c_str(), openFlags, openMode);
		if (fd == -1) throw runtime_error ("Failure " + to_string(errno) + " opening " + filename);
		int ok = dup2 (fd, dupFd);
		if (ok == -1) throw runtime_error ("Failure " + to_string(errno) + " duplicating " + filename);
		ok = close (fd);
		if (ok == -1) throw runtime_error ("Failure " + to_string(errno) + " closing " + filename);
	}
}*/

/** Redirect std/in/out/err to files named, if present */
/*static void redirectIO (bool isRestart, program::IO io) {
	redirectFd (io.in, O_RDONLY | O_CREAT, 0, STDIN_FILENO);
	redirectFd (io.out, O_WRONLY | O_CREAT | (isRestart ? O_APPEND : O_TRUNC), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, STDOUT_FILENO);
	redirectFd (io.err, O_WRONLY | O_CREAT | (isRestart ? O_APPEND : O_TRUNC), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, STDERR_FILENO);
}*/

static void dupFD (FD from, FD to) {
	if (from != to) {
		dupFd (from, to);
		closeFd (from);
	}
}

static void redirectStdInOutErr (program::IO io) {
	dupFD (io.in, STDIN_FILENO);
	dupFD (io.out, STDOUT_FILENO);
	dupFD (io.err, STDERR_FILENO);
}

/** Run prepCommand if clear is true, then start program. Redirect its stdin/out/err to supplied IO. */
pid_t program::start (bool clear, program::Program program, program::IO io) {
	if (clear) runCommand (program.prepCommand);
	char** args = argv (program);
	pid_t pid = fork ();
	if (pid == 0) { // This is the child process, execute the command
		ptrace (PT_TRACE_ME, 0, 0, 0); // cause SIGTRAP to be sent to parent on successful execvp below
		redirectStdInOutErr (io);
		execvp (program.executable.c_str(), args);
		_exit (errno);  // execvp only returns on error
	} else if (pid < 0)
		throw runtime_error ("Failed to launch: " + program.executable + program::optionsString (program.options));
	else { // This is the parent process
		int s;
		waitpid (pid, &s, 0); // wait for SIGTRAP (successful execvp) or exit (unsuccessful execvp)
		if (WIFEXITED(s))
			throw runtime_error ("Failed to launch (" + to_string(WEXITSTATUS(s)) + "): " + program.executable + program::optionsString (program.options));
		assert (WIFSTOPPED(s) && WSTOPSIG(s) == SIGTRAP);
		ptrace (PT_DETACH, pid, 0, 0);
		return pid;
	}
}
