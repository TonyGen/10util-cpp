/* Echo client and server
   Compile as:
     g++ sinksource.cpp -o sinksource -I/opt/local/include -L/opt/local/lib -lboost_system-mt -lboost_thread-mt -lboost_serialization-mt -l10util
   Run as:
     network server <port>
     network client <host> <port> <message> */

#include <10util/network.h>
#include <10util/util.h> // parse_string

using namespace std;

class Foo {
	friend ostream& operator<< (ostream& out, const Foo &a) {out << "Foo " << a.z; return out;}
public:
	float z;
	Foo (float z) : z(z) {}
	Foo () {}
};

class Bar {
	friend ostream& operator<< (ostream& out, const Bar &a) {out << "Bar " << a.x << " " << a.y; return out;}
public:
	string x;
	int y;
	Bar (string x, int y) : x(x), y(y) {}
	Bar () {}
};

namespace boost {
namespace serialization {

template <class Archive> void serialize (Archive &ar, Foo &a, const unsigned version) {
	ar & a.z;
}

template <class Archive> void serialize (Archive &ar, Bar &a, const unsigned version) {
	ar & a.x;
	ar & a.y;
}

}}

static Bar foobar (Foo f) {
	return Bar (to_string (f), f.z);
}

void mainClient (network::Host serverHost, network::Port serverPort, float r) {
	io::IOStream conn = network::connect (network::HostPort (serverHost, serverPort));
	io::SourceSink <Bar, Foo> ss (conn);
	ss << Foo (r);
	Bar bar;
	ss >> bar;
	cout << bar << endl;
}

void serve (io::IOStream conn) {
	io::SourceSink <Foo, Bar> ss (conn);
	Foo foo;
	ss >> foo;
	cout << foo << endl;
	ss << foobar (foo);
}

void mainServer (network::Port localPort) {
	cout << "listen on " << localPort << endl;
	boost::shared_ptr <boost::thread> t = network::listen (localPort, serve);
	t->join();  // wait forever
}

static std::string usage = "Try `sinksource server <port>` or `sinksource client <hostname> <port> <float>`";

int main (int argc, const char* argv[]) {
	if (argc == 3 && string(argv[1]) == "server")
		mainServer (parse_string <network::Port> (argv[2]));
	else if (argc == 5 && string(argv[1]) == "client")
		mainClient (argv[2], parse_string <network::Port> (argv[3]), parse_string <float> (argv[4]));
	else std::cerr << usage << std::endl;
}
