/* Untyped and typed IO streams */

#ifndef UTIL_IO_H_
#define UTIL_IO_H_

#include <iostream>
#include <boost/smart_ptr.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <sstream>

namespace io {

typedef boost::shared_ptr <std::istream> IStream;
typedef boost::shared_ptr <std::ostream> OStream;
typedef boost::shared_ptr <std::iostream> IOStream;

template <class A> struct Source {
	IStream in;
	Source (IStream in) : in(in) {}
	Source<A> & operator>> (A &value) {
		boost::archive::text_iarchive archive (*in);
		archive >> value;
		return *this;
	}
};

template <class A> struct Sink {
	OStream out;
	Sink (OStream out) : out(out) {}
	Sink<A> & operator<< (const A &value) {
		boost::archive::text_oarchive archive (*out);
		archive << value;
		out->flush();

		/* debug logging
		std::stringstream ss;
		boost::archive::text_oarchive ar (ss);
		ar << value;
		std::cout << "Sent: " << ss.str() << std::endl; */
		return *this;
	}
};

template <class I, class O> struct SourceSink {
	Source<I> source;
	Sink<O> sink;
	SourceSink (IOStream inout) : source (Source<I> (inout)), sink (Sink<O> (inout)) {}
	SourceSink (IStream in, OStream out) : source (Source<I> (in)), sink (Sink<O> (out)) {}
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
