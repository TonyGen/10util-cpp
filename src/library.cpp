
#include "library.h"

/** Load library */
library::Library library::load (Libname libname) {
	std::string err;
	void* lib = dlopen (("lib" + libname + ".so").c_str(), RTLD_LAZY);
	if (!lib) {
		err += std::string (dlerror());
		lib = dlopen (("lib" + libname + ".dylib").c_str(), RTLD_LAZY);
	}
	if (!lib) {
		err += "\n" + std::string (dlerror());
		throw std::runtime_error ("Cannot open library lib" + libname + ".so(.dylib):\n" + err);
	}
	return Library (libname, lib);
}

/** Same as `load` except don't return anything */
void library::load_ (Libname libname) {
	load (libname);
}
