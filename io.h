/* Untyped and typed IO streams */

#ifndef UTIL_IO_H_
#define UTIL_IO_H_

#include <iostream>
#include <boost/smart_ptr.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <sstream>

#define COUT (std::cout << boost::this_thread::get_id() << ": ")

namespace io {

typedef boost::shared_ptr <std::istream> IStream;
typedef boost::shared_ptr <std::ostream> OStream;
typedef boost::shared_ptr <std::iostream> IOStream;

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

class SourceException : public std::exception {
public:
	std::string message;
	std::string errorType;  // typically type name
	std::string errorMessage;
	SourceException (std::string message, const std::exception &e)
		: message(message), errorType (typeid(e).name()), errorMessage (std::string (e.what())) {}
	SourceException () {}  // for serialization
	~SourceException () throw () {}
	const char* what() const throw () {  // overriden
		std::stringstream ss;
		ss << "(" << errorType << ") " << errorMessage << ":\n\t" << message << std::endl;
		return ss.str().c_str();
	}
};

template <class A> struct Source {
	IStream in;
	Source (IStream in) : in(in) {}
	Source<A> & operator>> (A &value) {
		//boost::archive::text_iarchive archive (*in);
		//archive >> value;

		std::string mess;
		std::getline (*in, mess);
		// std::cout << "Received: " << mess << std::endl;
		try {value = deserialized<A> (mess);} catch (std::exception &e) {throw SourceException (mess, e);}

		return *this;
	}
};

template <class A> struct Sink {
	OStream out;
	Sink (OStream out) : out(out) {}
	Sink<A> & operator<< (const A &value) {
		//boost::archive::text_oarchive archive (*out);
		//archive << value;
		//out->flush();

		std::string mess = serialized (value);
		*out << mess << std::endl;
		// std::cout << "Sent: " << mess << std::endl;

		return *this;
	}
};

template <class I, class O> struct SourceSink {
	Source<I> source;
	Sink<O> sink;
	SourceSink (IOStream inout) : source (Source<I> (inout)), sink (Sink<O> (inout)) {}
	SourceSink (IStream in, OStream out) : source (Source<I> (in)), sink (Sink<O> (out)) {}
};

}

#endif /* IO_H_ */
