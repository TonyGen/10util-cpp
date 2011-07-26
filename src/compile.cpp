
#include "compile.h"
#include <cstdlib> // rand, system
#include <fstream>
#include "util.h" // to_string

/** Generate unique var name */
// TODO: make globally unique
std::string compile::freshName (std::string prefix) {
	static int nextId = rand();
	std::stringstream ss;
	ss << prefix << nextId ++;
	return ss.str();
}

/** Compile given C++ source code, link with given libraries (prefix and suffix excluded), and dynamically load it */
library::Library compile::compileLoad (LinkContext ctx, std::string code) {
	library::Libname filename = freshName ("_compiled_lib_");
	std::ofstream sourceFile ((filename + ".cpp").c_str());
	sourceFile << ctx.header() << "\n" << code;
	sourceFile.close();

	std::stringstream cmd;
	cmd << "g++ -pipe -fPIC -DBOOST_BUILD_PCH_ENABLED -DNDEBUG -fexceptions -O0 -Winvalid-pch -c -o " << filename << ".o " << filename << ".cpp";
	for (unsigned i = 0; i < ctx.includePaths.size(); i++) cmd << " -I" << ctx.includePaths[i];
	int exitStatus = system (cmd.str().c_str());
	if (exitStatus != 0) throw std::runtime_error (cmd.str() + "\n failed with exit status " + to_string (exitStatus));

	std::stringstream cmd1;
	cmd1 << "g++ -pipe -shared -rdynamic -o lib" << filename << ".so " << filename << ".o";
	for (unsigned i = 0; i < ctx.libPaths.size(); i++) cmd1 << " -L" << ctx.libPaths[i];
	for (unsigned i = 0; i < ctx.libNames.size(); i++) cmd1 << " -l" << ctx.libNames[i];
	exitStatus = system (cmd1.str().c_str());
	if (exitStatus != 0) throw std::runtime_error (cmd1.str() + "\n failed with exit status " + to_string (exitStatus));

	library::Library lib = library::load (filename);
	// remove ((filename + ".cpp").c_str());
	remove ((filename + ".o").c_str());
	return lib;
}

/** Compile given C++ code as a program, return its executable name */
std::string compile::compileProgram (LinkContext ctx, std::string code) {
	std::string filename = freshName ("_compiled_program_");
	std::ofstream sourceFile ((filename + ".cpp").c_str());
	sourceFile << ctx.header() << "\n" << code;
	sourceFile.close();

	std::stringstream cmd; cmd << "g++ -pipe -rdynamic -DBOOST_BUILD_PCH_ENABLED -DNDEBUG -fexceptions -O0 -Winvalid-pch -o " << filename << filename << ".cpp";
	for (unsigned i = 0; i < ctx.includePaths.size(); i++) cmd << " -I" << ctx.includePaths[i];
	for (unsigned i = 0; i < ctx.libPaths.size(); i++) cmd << " -L" << ctx.libPaths[i];
	for (unsigned i = 0; i < ctx.libNames.size(); i++) cmd << " -l" << ctx.libNames[i];
	int exitStatus = system (cmd.str().c_str());
	if (exitStatus != 0) throw std::runtime_error (cmd.str() + "\n failed with exit status " + to_string (exitStatus));
	return filename;
}

/** Execute statements in given link context.
 * Implementation: We wrap the expression in a function returning type T, append it to the header context, compile as shared library with library context as dependents, dynamically link in new shared library, and invoke wrapped function to get its value. */
void compile::exec (LinkContext ctx, std::string code) {
	typedef void (*Fun) ();
	std::string name = freshName ("_compiled_exec_");
	std::stringstream def;
	def << "extern \"C\" void " << name << " () {" << code << "}\n";
	library::Library lib = compileLoad (ctx, def.str());
	Fun fun = (Fun) dlsym (lib.handle, name.c_str());
	// remove (("lib" + lib.name + ".so").c_str());
	// remove (("lib" + lib.name + ".dylib").c_str());
	if (!fun) throw std::runtime_error (name + " not found, should not happen: " + dlerror());
	fun ();
}
