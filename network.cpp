
#include "network.h"
#include <boost/asio.hpp>
#include <map>
#include <utility> // pair

using boost::asio::ip::tcp;

/** Must be set at startup, if you want to interact on public network */
network::Hostname network::MyHostname = "localhost";

static boost::asio::io_service IO;

/** Accept client connections, running server on each one. Does not return. */
static void acceptLoop (boost::shared_ptr<tcp::acceptor> acceptor, boost::function1 <void, io::IOStream> server) {
	for (;;) {
		boost::shared_ptr<tcp::iostream> stream (new tcp::iostream);
		acceptor->accept (*stream->rdbuf());
		server (stream);
	}
}

/** Listen for client connections, forking a thread for each one running given server function.
 * Return listener thread that you may terminate to stop listening. */
boost::shared_ptr<boost::thread> network::listen (Port port, boost::function1 <void, io::IOStream> server) {
	boost::shared_ptr<tcp::acceptor> acceptor (new tcp::acceptor (IO, tcp::endpoint (tcp::v4(), port)));
	return boost::shared_ptr<boost::thread> (new boost::thread (boost::bind (acceptLoop, acceptor, server)));
}

/** Connect to the server listening on given hostPort */
io::IOStream network::connect (HostPort hostPort) {
	boost::shared_ptr<tcp::iostream> sock (new tcp::iostream (hostPort.hostname, to_string (hostPort.port)));
	if (! *sock) throw CantConnect (hostPort);
	return sock;
}

typedef std::map< network::HostPort, io::IOStream > Connections;

static boost::thread_specific_ptr<Connections> PersistentConnections;

/** Return persistent connection for this thread to given hostPort, creating one if first time for this thread or current connection is bad */
io::IOStream network::connection (HostPort hostPort) {
	if (! PersistentConnections.get()) PersistentConnections.reset (new Connections);
	Connections::iterator it = PersistentConnections->find (hostPort);
	if (it != PersistentConnections->end()) {
		if (it->second->good()) return it->second;
		else std::cout << "dropping bad persistent connection to " << hostPort << " and creating new one." << std::endl;
	}
	io::IOStream stream = connect (hostPort);
	PersistentConnections->insert (std::make_pair (hostPort, stream));
	return stream;
}
