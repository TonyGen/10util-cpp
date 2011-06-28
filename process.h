/** Executes job requests (threads and processes), keeps track of them, and allows you read their stdout/err, and kill them */

#pragma once

#include <signal.h>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>
#include "program.h"
#include "util.h" // to_string
#include "compile.h"

namespace process {

/** Process running a Program */
class Process_ {
public:
	program::Program program;
	unsigned id; // our id, not OS id
	pid_t pid; // OS id
	Process_ (program::Program program, unsigned id) : program(program), id(id) {}
	Process_ () {}  // for serialization
	std::string shortName () const {
		std::vector<std::string> parts;
		boost::split (parts, program.executable, boost::is_any_of ("/"));
		return *--parts.end() + "-" + to_string (id);
	}
	std::string outFilename () const {return shortName() + ".out";}
};

typedef boost::shared_ptr<Process_> Process;

/** Launch program with its stdout redirected to a local file named executable-id */
Process launch (program::Program program);

/** Restart program (does not run prepCommand first) */
void restart (Process deadProcess);

/** Wait for process to terminate returning its exit code */
int waitFor (Process process);

typedef int Signal;

/** Send signal to process. No-op if process dead */
void signal (Signal, Process);

/** Kill process. No-op if already dead */
void terminate (Process p);

/** Program process is running */
inline program::Program program (Process process) {return process->program;}

const compile::LinkContext module ("10util", "10util/process.h");
}

template <> inline compile::LinkContext typeModule<process::Process_> () {return process::module;}
template <> inline compile::LinkContext typeModule<program::Program> () {return compile::LinkContext ("10util", "10util/program.h");}

inline std::ostream& operator<< (std::ostream& out, const process::Process_& p) {out << p.shortName(); return out;}
