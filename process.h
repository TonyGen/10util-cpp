/** Executes job requests (threads and processes), keeps track of them, and allows you read their stdout/err, and kill them */

#ifndef PROCESS_H_
#define PROCESS_H_

#include <signal.h>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "program.h"
#include "unit.h"
#include "util.h" // to_string

namespace process {

/** Process running a Program */
class Process {
	friend std::ostream& operator<< (std::ostream& out, const Process& p) {out << p.shortName(); return out;}
public:
	program::Program program;
	unsigned id; // our id, not OS id
	pid_t pid; // OS id
	Process (program::Program program, unsigned id) : program(program), id(id) {}
	Process () {}  // for serialization
	std::string shortName () const {
		std::vector<std::string> parts;
		boost::split (parts, program.executable, boost::is_any_of ("/"));
		return *--parts.end() + "-" + to_string (id);
	}
	std::string outFilename () const {return shortName() + ".out";}
};

/** Launch program with its stdout redirected to a local file named executable-id */
Process launch (program::Program program);

/** Restart program (does not run prepCommand first) */
Unit restart (Process deadProcess);

/** Wait for process to terminate returning its exit code */
int waitFor (Process process);

typedef int Signal;

/** Send signal to process. No-op if process dead */
Unit signal (Signal, Process);

/** Kill process. No-op if already dead */
Unit terminate (Process p);

}

#endif /* PROCESS_H_ */
