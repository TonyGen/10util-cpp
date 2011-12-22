Install dependent library first in `/usr/local`:

- boost-dev
- boost-system-dev
- boost-thread-dev
- boost-filesystem-dev
- boost-serialization-dev
- boost-asio-dev

Download and remove '-ccp' suffix

	git clone git://github.com/TonyGen/10util-cpp.git 10util
	cd 10util

Build library `lib10util.so`

	scons

Install library in `/usr/local/lib` and header files in `/usr/local/include/10util`

	sudo scons install
