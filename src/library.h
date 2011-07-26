/* Dynamically loaded libraries */

#pragma once

#include <stdexcept>
#include <dlfcn.h>

namespace library {

/** Library name without lib prefix and suffix. eg. "m" for libm.so */
typedef std::string Libname;

/** Loaded library */
struct Library {
	Libname name;
	void* handle; // pointer to dynamically loaded code, use with dlsym
	Library (Libname name, void* handle) : name(name), handle(handle) {}
};

//TODO: library delete file method

/** Load library. If relative or absolute dir is supplied then find library there (dir MUST include trailing '/'), otherwise use runtime linker's search path (LD_LIBRARY_PATH) */
Library load (Libname libname, std::string dir = "");

/** Get address of named function or variable in given loaded library. F should be a function pointer or a pointer to the type of value that the variable holds. */
template <class F> F sym (Library library, std::string funName) {
	F ptr = (F) dlsym (library.handle, funName.c_str());
	if (!ptr) throw std::runtime_error (funName + " not found in lib" + library.name + ".so(.dylib): " + dlerror());
	return ptr;
}

}

/** INITIALIZE code is run when library it is defined in is loaded.
 * Note, the order of library initialization is undefined. In particular, library B can be initialized before library A even if B's initializer uses objects in A that need to be initialized. To avoid this wrap A's static variables inside accessor functions that B's initializer calls. */
#define INITIALIZE(code) \
struct _Initialize {_Initialize() {code}}; \
static _Initialize _;
