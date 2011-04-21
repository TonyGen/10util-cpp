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

template <class A> class Source {
	IStream in;
public:
	Source (IStream in) : in(in) {}
	Source<A> & operator>> (A &a) {
		boost::archive::text_iarchive arc (*in);
		arc >> a;
		return *this;
	}
};

template <class A> class Sink {
	OStream out;
public:
	Sink (OStream out) : out(out) {}
	Sink<A> & operator<< (const A &a) {
		boost::archive::text_oarchive arc (*out);
		arc << a;
		out->flush();
		return *this;
	}
};

template <class I, class O> class SourceSink {
	IOStream inout;
public:
	SourceSink<I,O> & operator>> (I &a) {
		boost::archive::text_iarchive arc (*inout);
		arc >> a;
		return *this;
	}
	SourceSink<I,O> & operator<< (const O &a) {
		boost::archive::text_oarchive arc (*inout);
		arc << a;
		inout->flush();
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
