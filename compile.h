/* Compile a C++ expression into a value */

#pragma once

#include <sstream>
#include <vector>
#include "library.h"
#include "util.h" // typeName

namespace compile {

/** Generate unique name */
std::string freshName (std::string prefix);

/** Invoke C++ compiler on given source code, link with given libraries (prefix and suffix excluded), then dynamically load it */
library::Library compileLoad (std::vector<library::Libname> dependentLibs, std::string code);

/** Compile expression with header in scope and dependent libraries linked in, and return its value. Expression must have type T. Header is source code with includes, macros, functions, etc. that the expression may need. Libraries are lib names without lib prefix and suffix (each lib name is added with -l option).
 * Implementation: We wrap the expression to a function returning type T, append it to the header, compile shared library with given libraries as dependents, dynamically link in new shared libary, and invoke wrapped function to get its value. */
template <class T> T eval (std::vector<library::Libname> dependentLibs, std::string header, std::string expr) {
	typedef T (*Fun) ();
	std::string name = freshName ("_compiled_value_");
	std::stringstream code;
	code << header << "\n\nextern \"C\" " << typeName<T>() << " " << name << " () {return " << expr << ";}\n";
	library::Library lib = compileLoad (dependentLibs, code.str());
	Fun fun = (Fun) dlsym (lib.handle, name.c_str());
	if (!fun) throw std::runtime_error (name + " not found, should not happen: " + dlerror());
	return fun();
}

/** Compile C++ code as a program, return its executable name */
std::string compileProgram (std::vector<library::Libname> dependentLibs, std::string code);

}
