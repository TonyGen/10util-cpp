/* Simple messaging interface on top of TCP sockets. Max message size is 2 ^ 32 - 1 bytes. */

#include "network.h"
#include <iostream>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>

using boost::asio::ip::tcp;

static boost::asio::io_service IO;

/** Connect to server listening on given hostname and port */
network::Socket network::connect (std::string hostname, unsigned short port) {
	tcp::resolver resolver (IO);
	tcp::resolver::query query (tcp::v4(), hostname, to_string (port));
	Socket sock (new tcp::socket (IO));
	sock->connect (* resolver.resolve (query));
	return sock;
}

/** When sent over the network, each message is prefixed with its length encoded as 4-byte little-endian */
static const unsigned IntAsBytesLength = 4;
static boost::array<char,4> intAsBytes (unsigned int n) {
	boost::array<char,4> bytes;
	bytes[0] = n & 0xff;
	bytes[1] = (n >> 8) & 0xff;
	bytes[2] = (n >> 16) & 0xff;
	bytes[3] = (n >> 24) & 0xff;
	return bytes;
}
static unsigned int bytesAsInt (boost::array<char,4> bytes) {
	int n = bytes[3];
	n = (n << 8) + bytes[2];
	n = (n << 8) + bytes[1];
	n = (n << 8) + bytes[0];
	return n;
}

/** Send message over socket */
void network::send (Socket sock, Message message) {
	boost::asio::write (*sock, boost::asio::buffer (intAsBytes (message.length)));
	boost::asio::write (*sock, boost::asio::buffer (message.data.get(), message.length));
}

/** Receive message from socket */
network::Message network::receive (Socket sock) {
	boost::array<char,IntAsBytesLength> bytes;
	read (*sock, boost::asio::buffer (bytes));
	unsigned int len = bytesAsInt (bytes);
	boost::shared_array<char> data (new char[len]);
	read (*sock, boost::asio::buffer (data.get(), len));
	return Message (data, len);
}

/** Listen for client connections and fork a thread for each one running server function */
void network::listen (unsigned short port, boost::function1 <void, Socket> server) {
	tcp::acceptor acceptor (IO, tcp::endpoint (tcp::v4(), port));
	for (;;) {
		Socket sock (new tcp::socket (IO));
		acceptor.accept (*sock);
		boost::thread th (boost::bind (server, sock));
	}
}
