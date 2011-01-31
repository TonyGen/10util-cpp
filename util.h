/* Miscellaneous helper functions -ajh */

#ifndef UTIL_H_
#define UTIL_H_

#include <boost/function.hpp>
#include <sstream>
#include <ostream>
#include <vector>
#include <map>

template <class T> inline std::string to_string (const T& t) {
	std::stringstream ss;
	ss << t;
	return ss.str();
}

inline std::vector <std::string> argsVector (int argc, char* argv[]) {
	std::vector <std::string> args;
	for (int i = 0; i < argc; i++) {
		args.push_back (argv[i]);
	}
	return args;
}

template <class A, class B> std::vector<B> fmap (boost::function1<B,A> fun, std::vector<A> list) {
	std::vector<B> newList;
	for (unsigned i = 0; i < list.size(); i++)
		newList.push_back (fun (list[i]));
	return newList;
}

template <class A, class B> std::vector<B> fmap (B (*fun) (A), std::vector<A> list) {
	boost::function1<B,A> f = fun;
	return fmap (f, list);
}

/** Recycle items until N listed */
template <class A> std::vector<A> cycle (unsigned n, std::vector<A> items) {
	unsigned c = items.size();
	if (c == 0 && n > 0) throw std::runtime_error ("can't cycle empty list");
	std::vector<A> list;
	while (true) {
		for (unsigned i = 0; i < c; i++) {
			if (list.size() == n) return list;
			list.push_back (items[i]);
		}
	}
}

/** Repeat item until N listed */
template <class A> std::vector<A> repeat (unsigned n, A item) {
	std::vector<A> list;
	for (unsigned i = 0; i < n; i++)
		list.push_back (item);
	return list;
}

/** pair elements from each list together, final list size equals size of smaller list, remaining elements in larger list are ignored */
template <class A, class B> std::vector< std::pair<A,B> > zip (std::vector<A> as, std::vector<B> bs) {
	unsigned n = as.size() < bs.size ? as.size() : bs.size();
	std::vector< std::pair<A,B> > pairs;
	for (unsigned i = 0; i < n; i++)
		pairs.push_back (std::make_pair (as[i], bs[i]));
	return pairs;
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

#endif /* UTIL_H_ */
