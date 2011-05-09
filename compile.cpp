
#include "compile.h"
#include <cstdlib> // rand, system
#include <fstream>

/** Generate unique var name */
// TODO: make globally unique
std::string compile::freshName (std::string prefix) {
	static int nextId = rand();
	std::stringstream ss;
	ss << prefix << nextId ++;
	return ss.str();
}

/** Compile given C++ source code, link with given libraries (prefix and suffix excluded), and dynamically load it */
compile::Library compile::compile (std::vector<std::string> dependentLibs, std::string code) {
	std::string filename = freshName ("_compile_code_");
	std::ofstream sourceFile ((filename + ".cpp").c_str()); sourceFile << code; sourceFile.close();

	std::stringstream cmd; cmd << "g++ -g -fPIC -c -I. -I/opt/local/include -o " << filename << ".o " << filename << ".cpp";
	int exitStatus = system (cmd.str().c_str());
	if (exitStatus != 0) throw std::runtime_error (cmd.str() + "\n failed with exit status " + to_string (exitStatus));

	std::stringstream cmd1; cmd1 << "g++ -shared -L. -L/opt/local/lib -o lib" << filename << ".so " << filename << ".o";
	for (unsigned i = 0; i < dependentLibs.size(); i++) cmd1 << " -l" << dependentLibs[i];
	exitStatus = system (cmd1.str().c_str());
	if (exitStatus != 0) throw std::runtime_error (cmd1.str() + "\n failed with exit status " + to_string (exitStatus));

	void* lib = dlopen (("lib" + filename + ".so").c_str(), RTLD_NOW);
	if (!lib) throw std::runtime_error ("Cannot open library lib" + filename + ".so: " + dlerror());
	return Library (filename, lib);
}
