
#include "library.h"

/** Load library */
library::Library library::load (Libname libname) {
	void* lib = dlopen (("lib" + libname + ".so").c_str(), RTLD_LAZY);
	if (!lib) lib = dlopen (("lib" + libname + ".dylib").c_str(), RTLD_LAZY);
	if (!lib) throw std::runtime_error ("Cannot open library lib" + libname + ".so(.dylib): " + dlerror());
	return Library (libname, lib);
}

/** Same as `load` except don't return anything */
void library::load_ (Libname libname) {
	load (libname);
}
