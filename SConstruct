libname = '10util'

lib = SharedLibrary (libname, Glob('*.cpp'),
	CCFLAGS = ['-g', '-rdynamic'],
	CPPPATH = ['.', '/opt/local/include'],
	LIBPATH = ['/opt/local/lib'],
	LIBS = Split ('dl boost_system-mt boost_thread-mt boost_filesystem-mt boost_serialization-mt') )

Alias ('install', '/usr/local')
Install ('/usr/local/lib', lib)
Install ('/usr/local/include/' + libname, Glob('*.h'))
