/* Miscellaneous helper function on file descriptors */

#ifndef UTIL_FILE_H_
#define UTIL_FILE_H_

#import <string>
#include <fcntl.h>

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

#endif /* UTIL_FILE_H_ */