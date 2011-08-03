
#include "process.h"
#include "file.h"
#include <sys/wait.h>
#include <signal.h>
#include <cerrno>
#include <boost/algorithm/string.hpp>

using namespace std;

template <> module::Module type<process::Process>::module = process::module;

/** Log filename is derived (using md5) from program name its options, so each program will have a different but reproducible log filename. Pid is not included because we don't want the log filename to change when the process is restarted. */
string process::logFilename (program::Program program) {
	return program::md5Name (program) + ".log";
}

namespace process {
Process start (bool clear, program::Program program) {
	string logFile = process::logFilename (program);
	FDW log = openFile (logFile, O_WRONLY | O_CREAT | (clear ? O_TRUNC : O_APPEND), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	program::IO io (0, log, log);
	pid_t pid = program::start (clear, program, io);
	closeFd (log); // child process already duplicated it
	cout << (clear ? "launch " : "restart ") << pid << " " << program << (clear ? " &> " : " &>> ") << logFile << endl;
	return process::Process (program, pid);
}
}

/** Launch program with its stdout redirected to a local file named executable-id */
process::Process process::launch (program::Program program) {return process::start (true, program);}

/** Restart program under same process object. Process must not already be running. */
process::Process process::restart (program::Program program) {return process::start (false, program);}

/** Wait for process to terminate returning its exit code */
int process::waitFor (Process process) {
	int status;
	waitpid (process.pid, &status, 0);
	return status;
}

void process::signal (Signal s, Process p) {kill (p.pid, s);}

/** Kill process. No-op if already dead */
void process::terminate (Process p) {signal (SIGTERM, p);}

static const program::Program ps ("", "ps", program::options ("e", "", "o", "pid=", "o", "command="));

static boost::optional<process::Process> parsePSLine (string line) {
	if (line.empty()) return boost::none;
	vector<string> args;
	boost::split (args, line, boost::is_any_of (" "));
	args.erase (remove (args.begin(), args.end(), ""), args.end());
	if (args.size() < 2) throw runtime_error (to_string (ps) + " produced unexpected output: " + line);
	pid_t pid = parse_string<pid_t> (args[0]);
	string executable = args[1];
	if (executable[0] == '(') return boost::none; // dead processes are surrounded by parens (at least on Mac OSX)
	args.erase (args.begin(), args.begin() + 2);
	program::Options options = program::parseArgs (args);
	return process::Process (program::Program ("", executable, options), pid);
}

/** Return all processes running on local machine. Note, they will be missing their program prepCommand. */
vector<process::Process> process::allProcesses () {
	Pipe pipe = makePipe();
	program::IO io (0, pipe.writeEnd, 2);
	pid_t pid = program::start (false, ps, io);
	closeFd (pipe.writeEnd);
	FILE* in = fdopen (pipe.readEnd, "r");
	vector<process::Process> procs;
	while (boost::optional<string> line = fGetLine (in)) {
		boost::optional<process::Process> p = parsePSLine (*line);
		if (p) procs.push_back (*p);
	}
	fclose (in);
	return procs;
}
