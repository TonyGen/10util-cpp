libname = '10util'

lib = SharedLibrary (libname, Glob('src/*.cpp'),
	CCFLAGS = ['-pg', '-rdynamic'],
	CPPPATH = ['.', '/usr/local/include'],
	LIBPATH = ['/usr/local/lib'],
	LIBS = Split ('dl crypto boost_system-mt boost_thread-mt boost_filesystem-mt boost_serialization-mt') )

Alias ('install', '/usr/local')
Install ('/usr/local/lib', lib)
Install ('/usr/local/include/' + libname, Glob('src/*.h'))
