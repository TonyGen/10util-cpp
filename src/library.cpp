
#include "library.h"

/** Load library. If dir is non-empty then find library there (dir MUST include trailing '/'), otherwise use runtime linker's search path (LD_LIBRARY_PATH) */
library::Library library::load (Libname libname, std::string dir) {
	std::string err;
	void* lib = dlopen ((dir + "lib" + libname + ".so").c_str(), RTLD_LAZY);
	if (!lib) {
		err += std::string (dlerror());
		lib = dlopen ((dir + "lib" + libname + ".dylib").c_str(), RTLD_LAZY);
	}
	if (!lib) {
		err += "\n" + std::string (dlerror());
		throw std::runtime_error ("Cannot open library " + dir + "lib" + libname + ".so(.dylib):\n" + err);
	}
	return Library (libname, lib);
}
