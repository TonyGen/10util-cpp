/* Echo client and server
   Compile as:
     g++ network.cpp -o network -I/opt/local/include -L/opt/local/lib -lboost_system-mt -lboost_thread-mt -lboost_serialization-mt -l10util
   Run as:
     network server <port>
     network client <host> <port> <message> */

#include <10util/network.h>
#include <10util/util.h> // parse_string

using namespace std;

void mainClient (network::Hostname serverHost, network::Port serverPort, string message) {
	io::IOStream conn = network::connect (network::HostPort (serverHost, serverPort));
	*conn << message << endl;
	string reply;
	getline (*conn, reply);
	cout << reply << endl;
}

void serve (io::IOStream conn) {
	string request;
	getline (*conn, request);
	cout << request << endl;
	*conn << request << endl;
}

void mainServer (network::Port localPort) {
	cout << "listen on " << localPort << endl;
	boost::shared_ptr <boost::thread> t = network::listen (localPort, serve);
	t->join();  // wait forever
}

static std::string usage = "Try `network server <port>` or `network client <hostname> <port> <message>`";

int main (int argc, const char* argv[]) {
	if (argc == 3 && string(argv[1]) == "server")
		mainServer (parse_string <network::Port> (argv[2]));
	else if (argc == 5 && string(argv[1]) == "client")
		mainClient (argv[2], parse_string <network::Port> (argv[3]), argv[4]);
	else std::cerr << usage << std::endl;
}
