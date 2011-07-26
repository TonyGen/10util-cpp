/* IO streams and serialization */

#pragma once

#include <iostream>
#include <boost/smart_ptr.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/thread.hpp>
#include <sstream>
#include "except.h"

// serialization for standard types
//#include <boost/serialization/serialization.hpp>
#include <boost/serialization/utility.hpp> // pair
#include <boost/serialization/vector.hpp>
#include <boost/serialization/variant.hpp>

#define COUT (std::cout << boost::this_thread::get_id() << ": ")

namespace io {

typedef boost::shared_ptr <std::istream> IStream;
typedef boost::shared_ptr <std::ostream> OStream;
typedef boost::shared_ptr <std::iostream> IOStream;

/** A Code is a serialized encoding of some object */
class Code {
	friend std::ostream& operator<< (std::ostream& out, const io::Code &x) {
		out << x.data.size() << " " << x.data;
		return out;
	}
	friend std::istream& operator>> (std::istream& in, io::Code &x) {
		size_t size;
		in >> size;
		in.ignore (1); // skip space
		char* buffer = new char [size];
		in.read (buffer, size);
		x.data.assign (buffer, size);
		delete[] buffer;
		return in;
	}
public:
	std::string data;
	Code (std::string data) : data(data) {}
	Code () {}
};

template <class A> A read (boost::archive::text_iarchive &ar) {
	A a;
	try {ar >> a;} catch (std::exception &e) {except::raise (e);}
	return a;
}

template <class A> void read (boost::archive::text_iarchive &ar, A &a) {
	try {ar >> a;} catch (std::exception &e) {except::raise (e);}
}

template <class A> void write (boost::archive::text_oarchive &ar, const A &a) {
	try {ar << a;} catch (std::exception &e) {except::raise (e);}
}

template <class A> A decode (Code x) {
	std::stringstream ss (x.data);
	boost::archive::text_iarchive ar (ss);
	A a;
	read (ar, a);
	return a;
}

template <class A> Code encode (A a) {
	std::stringstream ss;
	boost::archive::text_oarchive ar (ss);
	write (ar, a);
	return Code (ss.str());
}

template <class A> std::string show (A a) {
	std::stringstream ss;
	ss << a;
	return ss.str();
}

}

namespace boost {namespace serialization {

template <class Archive> void serialize (Archive & ar, ::io::Code & x, const unsigned version) {
	ar & x.data;
}

}}
