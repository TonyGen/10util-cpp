
#include "file.h"
#include "util.h" // to_string
#include <cerrno>
#include <unistd.h>
#include <stdexcept>

Pipe makePipe () {
	int pipeFds[2];
	int ok = pipe (pipeFds);
	if (ok == -1) throw std::runtime_error ("makePipe failed: " + to_string (errno));
	Pipe p;
	p.readEnd = pipeFds[0];
	p.writeEnd = pipeFds[1];
	return p;
}

FD openFile (std::string filename, int openFlags, int openMode) {
	FD fd = open (filename.c_str(), openFlags, openMode);
	if (fd == -1) throw std::runtime_error ("Failure opening " + filename + ": " + to_string(errno));
	return fd;
}

void closeFd (FD fd) {
	int ok = close (fd);
	if (ok == -1) throw std::runtime_error ("Failure closing FD " + to_string(fd) + ": " + to_string(errno));
}

void dupFd (FD from, FD to) {
	int ok = dup2 (from, to);
	if (ok == -1) throw std::runtime_error ("Failure duplicating FD from " + to_string(from) + " to " + to_string(to) + ": " + to_string(errno));
}
