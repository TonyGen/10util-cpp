
#include "process.h"
#include "file.h"

static void start (bool clear, process::Process proc) {
	FDW outW = openFile (proc.outFilename(), O_WRONLY | O_CREAT | (clear ? O_TRUNC : O_APPEND), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	program::IO io (0, outW, 2);
	proc.pid = program::start (clear, proc.program, io);
	closeFd (outW); // child process already duplicated it
}

static volatile unsigned nextProcessId;

/** Launch program with its stdout redirected to a local file named executable-id */
process::Process process::launch (program::Program program) {
	Process process (program, ++ nextProcessId);
	start (true, process);
	return process;
}

/** Restart program under same process object. Process must not already be running. */
Unit process::restart (Process process) {
	start (false, process);
	return unit;
}

/** Wait for process to terminate returning its exit code */
int process::waitFor (Process process) {
	int status;
	waitpid (process.pid, &status, 0);
	return status;
}

Unit process::signal (Signal s, Process p) {
	kill (p.pid, s);
	return unit;
}

/** Kill process. No-op if already dead */
Unit process::terminate (Process p) {signal (SIGTERM, p); return unit;}
