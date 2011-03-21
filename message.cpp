/* Simple messaging interface on top of TCP sockets. Max serialized message size is 2 ^ 32 - 1 bytes. */

#include "message.h"
#include <iostream>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

boost::array<char,4> _message::intAsBytes (unsigned int n) {
	boost::array<char,4> bytes;
	bytes[0] = n & 0xff;
	bytes[1] = (n >> 8) & 0xff;
	bytes[2] = (n >> 16) & 0xff;
	bytes[3] = (n >> 24) & 0xff;
	return bytes;
}

unsigned int _message::bytesAsInt (boost::array<char,4> bytes) {
	int n = bytes[3];
	n = (n << 8) + bytes[2];
	n = (n << 8) + bytes[1];
	n = (n << 8) + bytes[0];
	return n;
}

static boost::asio::io_service IO;

/** Listen for client connections, forking a thread for each one running given server function. Does not return. */
void message::listen (unsigned short port, boost::function1 <void, Socket> server) {
	tcp::acceptor acceptor (IO, tcp::endpoint (tcp::v4(), port));
	for (;;) {
		boost::shared_ptr <tcp::socket> sock (new tcp::socket (IO));
		acceptor.accept (*sock);
		boost::thread th (boost::bind (server, Socket (sock)));
	}
}

/** Connect to the server listening on given hostname and port */
message::Socket message::connect (std::string hostname, unsigned short port) {
	tcp::resolver resolver (IO);
	tcp::resolver::query query (tcp::v4(), hostname, to_string (port));
	boost::shared_ptr <tcp::socket> sock (new tcp::socket (IO));
	sock->connect (* resolver.resolve (query));
	return Socket (sock);
}
