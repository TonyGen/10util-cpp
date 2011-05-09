// Compile as: g++ compile.cpp -o compile -I/opt/local/include -L/opt/local/lib -lboost_system-mt -lboost_thread-mt -lboost_serialization-mt -l10util

#include <iostream>
#include <10util/compile.h>
#include <10util/vector.h>

using namespace std;

int main (int argc, const char* argv[]) {
	compile::Library lib = compile::compile (vector<string>(), string("int x = 3;\n"));
	int n = compile::eval<int> (items (lib.name), string("extern int x;"), string("x"));
	cout << n << endl;
}
