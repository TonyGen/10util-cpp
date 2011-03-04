/* Simple messaging interface on top of TCP sockets. Max message size is 2 ^ 32 - 1 bytes. */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/smart_ptr.hpp>
#include "util.h"

namespace network {

/** One end of a TCP connection to another machine. Created via `listen` or `connect`, closed via `close` */
typedef boost::shared_ptr<boost::asio::ip::tcp::socket> Socket;

/* Server - One thread per client connection running given server function. This server function can `receive` and `send` messages over the socket connected to the client. */
void listen (unsigned short port, boost::function1 <void, Socket> server);

/** Client - Connect to the designate server return the connected socket. You can then `send` and `receive` message from the server over this socket */
Socket connect (std::string hostname, unsigned short port);

/** Close connection */
void close (Socket);

struct Message {
	boost::shared_array<char> data;
	unsigned int length;  // num bytes in data. Max is 2 ^ 31 - 1 (to fit in 4 byte header when sent over socket)
	Message (boost::shared_array<char> data, unsigned int length) : data(data), length(length) {}
};

/** Send message to machine at other end of socket */
void send (Socket sock, Message message);

/** Receive message from machine at other end of socket */
Message receive (Socket sock);

}

#endif /* NETWORK_H_ */
