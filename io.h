/* Untyped and typed IO streams */

#ifndef UTIL_IO_H_
#define UTIL_IO_H_

#include <iostream>
#include <boost/smart_ptr.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <sstream>

namespace _io { // private namespace

template <class S, class X, class A> void readStream (S &stream, A &value) {
	X archive (stream);
	archive >> value;
}

template <class S, class X, class A> void writeStream (S &stream, const A &value) {
	X archive (stream);
	archive << value;
	stream.flush();

	/* debug logging
	std::stringstream ss;
	boost::archive::text_oarchive ar (ss);
	ar << value;
	std::cout << "Sent: " << ss.str() << std::endl; */
}

}

namespace io {

typedef boost::shared_ptr <std::istream> IStream;
typedef boost::shared_ptr <std::ostream> OStream;
typedef boost::shared_ptr <std::iostream> IOStream;

template <class A> class Source {
	IStream in;
public:
	Source (IStream in) : in(in) {}
	Source<A> & operator>> (A &a) {
		_io::readStream <std::istream, boost::archive::text_iarchive, A> (*in, a);
		return *this;
	}
};

template <class A> class Sink {
	OStream out;
public:
	Sink (OStream out) : out(out) {}
	Sink<A> & operator<< (const A &a) {
		_io::writeStream <std::ostream, boost::archive::text_oarchive, A> (*out, a);
		return *this;
	}
};

template <class I, class O> class SourceSink {
	IOStream inout;
public:
	SourceSink<I,O> & operator>> (I &a) {
		_io::readStream <std::iostream, boost::archive::text_iarchive, I> (*inout, a);
		return *this;
	}
	SourceSink<I,O> & operator<< (const O &a) {
		_io::writeStream <std::iostream, boost::archive::text_oarchive, O> (*inout, a);
		return *this;
	}
	SourceSink (IOStream inout) : inout(inout) {}
};

template <class A> A deserialized (std::string s) {
	std::stringstream ss (s);
	boost::archive::text_iarchive ar (ss);
	A a;
	ar >> a;
	return a;
}

template <class A> std::string serialized (A a) {
	std::stringstream ss;
	boost::archive::text_oarchive ar (ss);
	ar << a;
	return ss.str();
}

}

#endif /* IO_H_ */
