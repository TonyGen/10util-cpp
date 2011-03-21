/* Simple messaging interface on top of TCP sockets. Not thread safe. Max serialized message size is 2 ^ 32 - 1 bytes. */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/array.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "util.h"

/** Private */
namespace _message {

/** When sent over the network, each message is prefixed with its length encoded as 4-byte little-endian */
const unsigned IntAsBytesLength = 4;

/** When sent over the network, each message is prefixed with its length encoded as 4-byte little-endian */
boost::array<char,4> intAsBytes (unsigned int n);

unsigned int bytesAsInt (boost::array<char,4> bytes);

}

/** Public */
namespace message {

/** One end of a TCP connection. Created via `listen` or `connect`. Destructor closes connection. */
struct Socket {
// Really private, but public for functions in this source file only
	boost::shared_ptr <boost::asio::ip::tcp::socket> sock;
	Socket (boost::shared_ptr <boost::asio::ip::tcp::socket> sock) : sock(sock) {}
};

/* Server - One thread per client connection running given server function. This function does not return. The server function can `receive` and `send` messages over the supplied socket connected to the client. */
void listen (unsigned short port, boost::function1 <void, Socket> server);

/** Client - Connect to the designate server. You can then `send` and `receive` messages over this socket */
Socket connect (std::string hostname, unsigned short port);

/** Send message to machine at other end of socket. Not thread safe.
 * A must be boost serializable */
template <class A> void send (Socket socket, A message) {
	std::stringstream ss;
	boost::archive::text_oarchive ar (ss);
	ar << message;
	std::string mess = ss.str();

	boost::asio::write (*socket.sock, boost::asio::buffer (_message::intAsBytes (mess.length())));
	boost::asio::write (*socket.sock, boost::asio::buffer (mess.c_str(), mess.length()));
}

/** Receive message from machine at other end of socket. Not thread safe. Message expected to be of type A (crash otherwise).
 * A must be boost serializable */
template <class A> A receive (Socket socket) {
	boost::array<char,_message::IntAsBytesLength> bytes;
	boost::asio::read (*socket.sock, boost::asio::buffer (bytes));
	unsigned int len = _message::bytesAsInt (bytes);
	char* data = new char[len];
	boost::asio::read (*socket.sock, boost::asio::buffer (data, len));
	std::string s (data, len);

	std::stringstream ss (s);
	boost::archive::text_iarchive ar (ss);
	A message;
	ar >> message;
	return message;
}

}

#endif /* MESSAGE_H_ */
