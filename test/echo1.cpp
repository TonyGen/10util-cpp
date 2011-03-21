/* Echo client and server */
/* Assumes util library has been built and installed in /usr/local/include and /usr/local/lib.
 * Compile as: g++ echo1.cpp -o echo1 -I/opt/local/include -L/opt/local/lib -lboost_system-mt -lboost_thread-mt -lboost_serialization-mt -l10util
 * Run as: `echo server <port>` and `echo client <hostname> <port> <message>` */

#include <iostream>
#include <10util/call.h>

using namespace std;

void mainClient (string serverHost, unsigned short serverPort, string message) {
	call::Socket <string, string> sock = call::connect (serverHost, call::Port <string, string> (serverPort));
	string reply = call::call (sock, message);
	cout << reply << endl;
}

static string respond (string req) {
	cout << req << endl;
	return req;
}

void mainServer (unsigned short localPort) {
	cout << "listen on " << localPort << endl;
	call::listen (call::Port <string, string> (localPort), respond);
}

static string usage = "Try `echo server <port>` or `echo client <hostname> <port> <message>`";

int main (int argc, const char* argv[]) {
	if (argc == 3 && string(argv[1]) == "server")
		mainServer (parse_string<unsigned short> (argv[2]));
	else if (argc == 5 && string(argv[1]) == "client")
		mainClient (argv[2], parse_string<unsigned short> (argv[3]), argv[4]);
	else cerr << usage << endl;
}
