/* Program (executable) launching with possible redirection of stdin/out/err */

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include "file.h"  // FD types
#include "util.h" // print pair & vector

namespace program {

	/** Command-line options. Non-empty keys are prefixed with "--", or "-" if single char, when printed, unless already has a "-" prefix. Use empty key for direct arguments */
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
	std::string optionsString (const Options &programOptions);

	/** Parse cmd-line args into options association list. Note: a direct arg following a no-arg option will be interpreted as the arg of the option. This is tolerable since unparsing (optionString) will reproduce the same line, but it will fool lookup and merge, so don't use these in this case. */
	Options parseArgs (std::vector<std::string> args);

	/** Program with options, ready to run */
	class Program {
		friend bool operator== (const Program& a, const Program& b) {return a.prepCommand == b.prepCommand && a.executable == b.executable && a.options == b.options;}
		friend bool operator< (const Program& a, const Program& b) {return a.prepCommand < b.prepCommand || (a.prepCommand == b.prepCommand && (a.executable < b.executable || (a.executable == b.executable && a.options < b.options)));}
		friend bool operator!= (const Program& a, const Program& b) {return !(a == b);}
		friend bool operator> (const Program& a, const Program& b) {return b < a;}
		friend bool operator>= (const Program& a, const Program& b) {return !(a < b);}
		friend bool operator<= (const Program& a, const Program& b) {return !(a > b);}
	public:
		std::string prepCommand;  // Executed before running program (must finish) upon fresh/clear start
		std::string executable;  // Program name, PATH is searched
		Options options;  // Command-line arguments supplied to executable
		Program (std::string prepCommand, std::string executable, Options options) :
			prepCommand(prepCommand), executable(executable), options(options) {}
		Program () {}
		/** Executable plus command-line arguments */
		std::string command() {return executable + " " + program::optionsString (options);}
		/** Filename of executable excluding path */
		std::string executableName () const;
	};

	/** MD5 hex string derived (using md5) from program name its options, so each program will have a different but reproducible hex string. */
	std::string md5Name (Program);

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

/* Printing & Serialization */

inline std::ostream& operator<< (std::ostream& out, const program::Program &p) {
	out << p.executable << " " << program::optionsString (p.options); return out;}

namespace boost {namespace serialization {

template <class Archive> void serialize (Archive & ar, program::Program & x, const unsigned version) {
	ar & x.prepCommand;
	ar & x.executable;
	ar & x.options;
}

}}
