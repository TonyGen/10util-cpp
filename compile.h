/* Compile a C++ expression into a value */

#pragma once

#include <sstream>
#include <vector>
#include "library.h"
#include "type.h"
#include <cstdio>

namespace compile {

/** Generate unique name */
std::string freshName (std::string prefix);

/** Dependent libraries and headers */
class LinkContext {
public:
	std::vector <std::string> libPaths; // g++ -L option
	std::vector <library::Libname> libNames; // g++ -l option
	std::vector <std::string> includePaths; // g++ -I option
	std::vector <std::string> headers; // lines prepended to source code, usually contains #include directives
	LinkContext () {}
	/** Header lines concatenated together */
	std::string header() {
		std::stringstream ss;
		for (unsigned i = 0; i < headers.size(); i++) ss << headers[i] << "\n";
		return ss.str();
	}
	void clearAll () {
		libPaths.clear();
		libNames.clear();
		includePaths.clear();
		headers.clear();
	}
};

/** Invoke C++ compiler on given source code in given link context, then dynamically load it */
library::Library compileLoad (LinkContext ctx, std::string code);

/** Execute statements in given link context */
void exec (LinkContext ctx, std::string code);

/** Evaluate expression in given link context, and return its value. Expression must have type T */
/* Implementation: Same as `exec` except prefix expr with 'return' and return result. */
template <class T> T eval (LinkContext ctx, std::string expr) {
	typedef T (*Fun) ();
	std::string name = freshName ("_compiled_value_");
	std::stringstream code;
	code << "extern \"C\" " << typeName<T>() << " " << name << " () {return " << expr << ";}\n";
	library::Library lib = compileLoad (ctx, code.str());
	Fun fun = (Fun) dlsym (lib.handle, name.c_str());
	remove (("lib" + lib.name + ".so").c_str());
	remove (("lib" + lib.name + ".dylib").c_str());
	if (!fun) throw std::runtime_error (name + " not found, should not happen: " + dlerror());
	return fun ();
}

/** Compile C++ code as a program, return its executable name */
std::string compileProgram (LinkContext ctx, std::string code);

}
