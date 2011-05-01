/* TCP client and server connections */

#pragma once

#include "io.h"
#include <boost/thread.hpp>
#include <boost/function.hpp>
#include "util.h" // to_string

namespace network {

typedef std::string Hostname;
typedef unsigned short Port;

class HostPort {
	friend std::ostream& operator<< (std::ostream& out, const HostPort &x) {out << x.hostname << ":" << x.port; return out;}
	friend bool operator== (const HostPort& a, const HostPort& b) {return a.hostname == b.hostname && a.port == b.port;}
	friend bool operator< (const HostPort& a, const HostPort& b) {return a.hostname < b.hostname ? true : (a.hostname > b.hostname ? false : a.port < b.port);}
	friend bool operator!= (const HostPort& a, const HostPort& b) {return !(a == b);}
	friend bool operator> (const HostPort& a, const HostPort& b) {return b < a;}
	friend bool operator>= (const HostPort& a, const HostPort& b) {return !(a < b);}
	friend bool operator<= (const HostPort& a, const HostPort& b) {return !(a > b);}
public:
	Hostname hostname;
	Port port;
	HostPort (Hostname hostname, Port port) : hostname(hostname), port(port) {}
};

/** Hostname that this process listens on and advertises. Must be set by user at startup. */
extern Hostname MyHostname;

/** Fork a thread that listens for client connections on given port. This thread runs given server function on every new connection. server should fork if wants to do a long running process. Kill returned listener thread when done listening. */
boost::shared_ptr <boost::thread> listen (Port port, boost::function1 <void, io::IOStream> server);

class CantConnect : public std::exception {
public:
	HostPort host;
	CantConnect (HostPort host) : host(host) {}
	~CantConnect () throw () {}
	const char* what() const throw () {  // overriden
		return ("Can't connect to " + to_string(host)) .c_str();
	}
};

/** Connect to given server, returning connection stream */
io::IOStream connect (HostPort hostPort);

/** Return persistent connection for this thread to given hostPort, creating one if first time for this thread */
io::IOStream connection (HostPort hostPort);

}
