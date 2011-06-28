/* Identify a "module" in a library. A module defines a function/class and is composed of the header file that declares the function/class, and the library that defines it or dependent libraries if the function/class is a template. */

#pragma once

#include "vector.h"
#include "util.h" // output vector

namespace module {

class Module {
	friend bool operator== (const Module& a, const Module& b) {return a.libPaths == b.libPaths && a.includePaths == b.includePaths && a.libNames == b.libNames && a.includeFiles == b.includeFiles;}
	friend bool operator< (const Module& a, const Module& b) {return a.libPaths < b.libPaths || (a.libPaths == b.libPaths && (a.includePaths < b.includePaths || (a.includePaths == b.includePaths && (a.libNames < b.libNames || (a.libNames == b.libNames && a.includeFiles < b.includeFiles)))));}
	friend bool operator!= (const Module& a, const Module& b) {return !(a == b);}
	friend bool operator> (const Module& a, const Module& b) {return b < a;}
	friend bool operator>= (const Module& a, const Module& b) {return !(a < b);}
	friend bool operator<= (const Module& a, const Module& b) {return !(a > b);}
public:
	std::vector <std::string> libPaths; // g++ -L option
	std::vector <std::string> includePaths; // g++ -I option
	std::vector <std::string> libNames; // g++ -l option
	std::vector <std::string> includeFiles; // header file names (ie. #include <IncludeFile>)
	Module (std::string libPath, std::string includePath, std::vector<std::string> libNames, std::string includeFile)
		: libPaths (items (libPath)), includePaths (items (includePath)), libNames (libNames), includeFiles (items (includeFile)) {}
	Module (std::string libPath, std::string includePath, std::string libName, std::string includeFile)
		: libPaths (items (libPath)), includePaths (items (includePath)), libNames (items (libName)), includeFiles (items (includeFile)) {}
	Module (std::vector<std::string> libNames, std::string includeFile) : libNames (libNames), includeFiles (items (includeFile)) {}
	Module (std::string libName, std::string includeFile) : libNames (items (libName)), includeFiles (items (includeFile)) {}
	Module () {} // for serialization
	/** concatenate this with arg */
	Module operator+ (const Module &ctx) const;
};

/** Append all their items together */
Module concat (std::vector<Module>);

}

/** Module where type is defined. Must specialize for each type */
template <typename T> struct type {static module::Module module;};
template <template <typename> class F> struct type1 {static module::Module module;};


std::ostream& operator<< (std::ostream& out, const module::Module &x);

namespace boost {namespace serialization {
template <class Archive> void serialize (Archive & ar, module::Module & x, const unsigned version) {
	ar & x.libPaths;
	ar & x.libNames;
	ar & x.includePaths;
	ar & x.includeFiles;
}
}}
