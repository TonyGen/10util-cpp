Download and remove '-ccp' suffix
	git clone git://github.com/TonyGen/util-cpp.git util
	cd util

Build library `libutil.a`
	scons

Install library in `/usr/local/lib` and header files in `/usr/local/include/util`
	sudo scons install
