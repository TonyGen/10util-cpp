// Compile as: g++ -I/opt/local/include compile.cpp -o compile -l10util

#include <iostream>
#include <10util/compile.h>
#include <10util/vector.h>

using namespace std;

int main (int argc, const char* argv[]) {
	library::Library lib = compile::compileLoad (vector<string>(), "int x = 42;");
	int n = compile::eval<int> (
		vector<string>(),
		"#include <stdexcept>\n"
		"#include <dlfcn.h>\n"
		"extern int x;\n"
		"int foo() {\n"
		"	void* lib = dlopen (\"" + lib.name + "\", RTLD_NOW | RTLD_GLOBAL);\n"
		"	if (!lib) throw std::runtime_error (dlerror());\n"
		"	return x;}",
		"foo()");
	cout << n << endl;
}
