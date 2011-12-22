// Compile as: g++ -I/usr/local/include compile.cpp -o compile -l10util

#include <iostream>
#include <10util/compile.h>

using namespace std;

int main (int argc, const char* argv[]) {
	compile::LinkContext ctx;
	library::Library lib = compile::compileLoad (ctx, "int x = 42;");
	ctx.libPaths.push_back(".");
	ctx.libNames.push_back (lib.name);
	ctx.headers.push_back("extern int x;");
	int n = compile::eval<int> (ctx, "x");
	cout << n << endl;
}
