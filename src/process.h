/** Executes job requests (threads and processes), keeps track of them, and allows you read their stdout/err, and kill them */

#pragma once

#include <signal.h>
#include <iostream>
#include "program.h"
#include "util.h" // to_string
#include "module.h"

namespace process {

const module::Module module ("10util", "10util/process.h");

struct Process {
	program::Program program;
	pid_t pid;
	Process (program::Program program, pid_t pid) : program(program), pid(pid) {}
	Process () {} // for serialization
};

/** Log filename is derived (using md5) from program name its options, so each program will have a different but reproducible log filename. Pid is not included because we don't want the log filename to change when the process is restarted. */
std::string logFilename (program::Program);

/** Launch program with its stdout redirected to a local file named executable-id */
Process launch (program::Program);

/** Restart program (does not run prepCommand first) */
Process restart (program::Program);

/** Wait for process to terminate returning its exit code */
int waitFor (Process);

typedef int Signal;

/** Send signal to process. No-op if process dead */
void signal (Signal, Process);

/** Kill process. No-op if already dead */
void terminate (Process);

/** Return all processes running on local machine. They will be missing their program prepCommand. */
std::vector<Process> allProcesses ();

}

/* Printing & Serialization */

inline std::ostream& operator<< (std::ostream& out, const process::Process& p) {
	out << p.pid << " " << p.program; return out;}

namespace boost {namespace serialization {

template <class Archive> void serialize (Archive & ar, process::Process & x, const unsigned version) {
	ar & x.program;
	ar & x.pid;
}

}}
