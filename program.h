/* */

#ifndef PROGRAM_H_
#define PROGRAM_H_

#include <string>
#include <vector>
#include <utility>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <10util/file.h>  // FD types

namespace program {

	/** Command-line options. Non-empty keys are prefixed with "--" when printed, unless already has a "-" prefix. Use empty key for direct arguments */
	typedef std::vector <std::pair <std::string, std::string> > Options;

	/** Short hand for options list of one pair */
	Options options (std::string name, std::string value);
	/** Short hand for options list of two pairs */
	Options options (std::string name1, std::string value1, std::string name2, std::string value2);
	/** Short hand for options list of three pairs */
	Options options (std::string name1, std::string value1, std::string name2, std::string value2, std::string name3, std::string value3);

	/** Merge first program options into second, i.e. second arg has precedence */
	Options merge (Options secondary, Options primary);

	/** Return value of key, or NULL if missing */
	boost::optional<std::string> lookup (std::string key, Options options);

	/** Flatten with space separation while prefixing non-empty keys with "--" unless they already start with "-" */
	std::string optionsString (Options programOptions);

	/** Program with options, ready to run */
	struct Program {
		std::string prepCommand;  // Executed before running program (must finish) upon fresh/clear start
		std::string executable;  // Program name, PATH is searched
		Options options;  // Command-line arguments supplied to executable
	};

	/** File descriptors for program input, output, and error. Use 0/1/2 for stdin/out/err */
	struct IO {
		FDR in;
		FDW out;
		FDW err;
		IO (FDR in, FDW out, FDW err) : in(in), out(out), err(err) {}
		IO () : in(0), out(1), err(2) {}  // defaults to stdin, stdout, stderr
	};

	/** Run prepCommand if clear is true, then start program. Redirect its stdin/out/err to supplied IO. Return its OS pid */
	pid_t start (bool clear, Program program, IO io = IO());

}

std::ostream & operator<< (std::ostream &, const program::Program &);
std::ostream & operator<< (std::ostream &, const program::IO &);

#endif /* PROGRAM_H_ */
