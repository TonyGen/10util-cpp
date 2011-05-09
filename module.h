/* Associate dependent libraries with header files, so a user knows which libraries to link with when including a header file. Each header is responsible for registering which libraries it depends on. */

#pragma once

#include <string>
#include <vector>
#include "vector.h" // items, for users

namespace module {

typedef std::string Lib; // without lib prefix or suffix
typedef std::string Header; // header file name without filepath, just what you give to #include

/** Return dependent libraries of given header, ie. those that you must link with when including given header */
std::vector<Lib> libraries (Header header);

/** Register that the given header requires the user links with the given libraries when including the header */
void registerLibraries (Header header, std::vector<Lib> libs);

/** Define one of these in the top leve of your header file to automatically register its dependent libraries */
struct DependentLibraries {
	DependentLibraries (Header header, std::vector<Lib> libs) {
		registerLibraries (header, libs);
	}
	DependentLibraries (const char* header, std::vector<const char*> libs) {
		std::vector<Lib> list;
		for (unsigned i = 0; i < libs.size(); i++) list.push_back (std::string (libs[i]));
		registerLibraries (std::string (header), list);
	}
};

}
