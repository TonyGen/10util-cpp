Remove '-ccp' suffix when downloading
	git clone git://github.com/TonyGen/util-cpp.git util

This library contains only header files for now, grouped into a `util` directory.
To install this directory in `/usr/local/include` do
	sudo scons install

To include in your program do
	#include <util/X>
where X is one of the header files in that directory like `mvar.h`
