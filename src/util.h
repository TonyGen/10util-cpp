/* Miscellaneous helper functions -ajh */

#pragma once

#include <boost/function.hpp>
#include <sstream>
#include <ostream>
#include <utility> // pair
#include <vector>
#include <map>
#include <stdexcept>
#include "type.h"

/** Convert bytes to its hexidecimal string form */
std::string hexString (unsigned char* bytes, unsigned bytesLen);

/** Return md5 hash of string as hex string */
std::string md5 (std::string);

inline std::vector <std::string> argsVector (int argc, char* argv[]) {
	std::vector <std::string> args;
	for (int i = 0; i < argc; i++) {
		args.push_back (argv[i]);
	}
	return args;
}

template <class K, class V> std::ostream& operator<< (std::ostream& out, const std::pair<K,V>& pair) {
	out << "(" << pair.first << ", " << pair.second << ")";
}

template <class T> std::ostream& operator<< (std::ostream& out, const std::vector<T>& list) {
	out << "[";
	for (unsigned i = 0; i < list.size(); i++) {
		out << list[i];
		if (i < list.size() - 1) out << ", ";
	}
	out << "]";
	return out;
}

template <class K, class V> std::ostream& operator<< (std::ostream& out, const std::map<K,V>& dict) {
	out << "{";
	for (typename std::map<K,V>::const_iterator it = dict.begin(); it != dict.end();) {
		out << it->first << ": " << it->second;
		++it;
		if (it == dict.end()) break;
		out << ", ";
	}
	out << "}";
	return out;
}

template <class T> std::string toString (const T& t) {
	std::stringstream ss;
	ss << t;
	return ss.str();
}
template <class T> std::string to_string (const T& t) {return toString(t);}

template <class T> T parse_string (std::string str) {
	std::istringstream ss (str);
	T a;
	if ((ss >> a).fail()) throw std::runtime_error ("error parsing \"" + str + "\" as type " + typeName(a));
	return a;
}

/** Tokenize string using given char as delimeter */
std::vector <std::string> split_string (char delimeter, std::string str);
