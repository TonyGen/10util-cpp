Download and remove '-ccp' suffix
	git clone git://github.com/TonyGen/10util-cpp.git 10util
	cd 10util

Build library `lib10util.a`
	scons

Install library in `/usr/local/lib` and header files in `/usr/local/include/util`
	sudo scons install
