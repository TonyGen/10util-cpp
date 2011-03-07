/* Echo client and server */
/* Assumes util library has been built and installed in /usr/local/include and /usr/local/lib.
 * Compile as: g++ echo.cpp -o echo -I/opt/local/include -L/opt/local/lib -lboost_system-mt.
 * Run as: `echo server <port>` and `echo client <hostname> <port> <message>` */

#include <iostream>
#include <10util/network.h>

void mainClient (std::string serverHost, unsigned short serverPort, std::string message) {
	network::Socket sock = network::connect (serverHost, serverPort);
	boost::shared_array<char> data (new char [message.size() + 1]);
	std::strcpy (data.get(), message.c_str());
	network::send (sock, network::Message (data, message.size() + 1));
	network::Message mess = network::receive (sock);
	std::cout << mess.data.get() << std::endl;
}

void serve (network::Socket sock) {
	network::Message mess = network::receive (sock);
	std::cout << mess.data.get() << std::endl;
	network::send (sock, mess);
}

void mainServer (unsigned short localPort) {
	std::cout << "listen on " << localPort << std::endl;
	network::listen (localPort, serve);
}

static std::string usage = "Try `echo server <port>` or `echo client <hostname> <port> <message>`";

int main (int argc, const char* argv[]) {
	if (argc == 3 && std::string(argv[1]) == "server")
		mainServer (parse_string<unsigned short> (argv[2]));
	else if (argc == 5 && std::string(argv[1]) == "client")
		mainClient (argv[2], parse_string<unsigned short> (argv[3]), argv[4]);
	else std::cerr << usage << std::endl;
}
