/* Miscellaneous helper function on file descriptors */

#pragma once

#include <string>
#include <fcntl.h>
#include <cstdio>
#include <boost/optional.hpp>

typedef int FD; // File descriptor
typedef FD FDW; // Write-only file descriptor
typedef FD FDR; // Read-only file descriptor

struct Pipe {
	FDR readEnd;
	FDW writeEnd;
};

Pipe makePipe ();

/** Throw exception on error */
FD openFile (std::string filename, int openFlags, int openMode);

/** Throw exception on error */
void closeFd (FD fd);

/** Throw exception on error */
void dupFd (FD from, FD to);

/** Convert FD to FILE*. mode must be same that FD was open it ("r" or "w") */
// FILE* fdopen (FD fd, const char *mode);

// fclose (FILE*)

/** Return next line or NULL if EOF */
boost::optional<std::string> fGetLine (FILE* file);
