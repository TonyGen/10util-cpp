/* Compile a C++ expression into a value */

#pragma once

#include <sstream>
#include <vector>
#include "library.h"
#include "type.h"
#include <cstdio>
#include "vector.h"
#include "util.h" // output vector

namespace compile {

inline std::string include (std::string headName) {return "#include <" + headName + ">";}

/** Generate unique name */
std::string freshName (std::string prefix);

/** Dependent libraries and headers */
class LinkContext {
	friend bool operator== (const LinkContext& a, const LinkContext& b) {return a.libPaths == b.libPaths && a.includePaths == b.includePaths && a.libNames == b.libNames && a.headers == b.headers;}
	friend bool operator< (const LinkContext& a, const LinkContext& b) {return a.libPaths < b.libPaths || (a.libPaths == b.libPaths && (a.includePaths < b.includePaths || (a.includePaths == b.includePaths && (a.libNames < b.libNames || (a.libNames == b.libNames && a.headers < b.headers)))));}
	friend bool operator!= (const LinkContext& a, const LinkContext& b) {return !(a == b);}
	friend bool operator> (const LinkContext& a, const LinkContext& b) {return b < a;}
	friend bool operator>= (const LinkContext& a, const LinkContext& b) {return !(a < b);}
	friend bool operator<= (const LinkContext& a, const LinkContext& b) {return !(a > b);}
public:
	std::vector <std::string> libPaths; // g++ -L option
	std::vector <std::string> includePaths; // g++ -I option
	std::vector <library::Libname> libNames; // g++ -l option
	std::vector <std::string> headers; // lines prepended to source code, usually contains #include directives
	LinkContext (library::Libname libName, std::string headName) : libNames(items(libName)), headers(items(include(headName))) {}
	LinkContext (std::vector<library::Libname> libNames, std::vector<std::string> headNames) : libNames(libNames), headers(fmap(include,headNames)) {}
	LinkContext (std::string libPath, std::string includePath, std::vector<library::Libname> libNames, std::vector<std::string> headNames) : libPaths(items(libPath)), includePaths(items(includePath)), libNames(libNames), headers(fmap(include,headNames)) {}
	LinkContext () {}
	/** concatenate this with arg */
	LinkContext operator+ (const LinkContext &ctx) const;
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

/** Append all their items together */
LinkContext joinContexts (std::vector<LinkContext> ctxs);

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

inline std::ostream& operator<< (std::ostream& out, const compile::LinkContext &x) {
	out << "LinkContext " << x.libPaths << ", " << x.libNames << ", " << x.includePaths << ", " << x.headers;
	return out;
}
