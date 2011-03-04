/* Miscellaneous helper functions -ajh */

#ifndef UTIL_H_
#define UTIL_H_

#include <boost/function.hpp>
#include <sstream>
#include <ostream>
#include <vector>
#include <map>

/** Unit type is like void but its single value can be passed between functions; useful for generic functions */
struct Unit {
	friend std::ostream& operator<< (std::ostream& out, const Unit& x) {out << "unit"; return out;}
};
extern Unit unit;

inline std::vector <std::string> argsVector (int argc, char* argv[]) {
	std::vector <std::string> args;
	for (int i = 0; i < argc; i++) {
		args.push_back (argv[i]);
	}
	return args;
}

template <class A> void push_all (std::vector<A>& list, const std::vector<A>& elems) {
	for (unsigned i = 0; i < elems.size(); i++)
		list.push_back (elems[i]);
}

template <class A, class B> B foldl (boost::function2<B,B,A> fun, B init, std::vector<A> list) {
	B val = init;
	for (unsigned i = 0; i < list.size(); i++)
		val = fun (val, list[i]);
	return val;
}
template <class A, class B> B foldl (B (*fun) (B, A), B init, std::vector<A> list) {
	boost::function2<B,B,A> f = fun;
	return foldl (f, init, list);
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

template <class A> std::vector<A> filter (boost::function1<bool,A> pred, std::vector<A> list) {
	std::vector<A> newList;
	for (unsigned i = 0; i < list.size(); i++)
		if (pred (list[i])) newList.push_back (list[i]);
	return newList;
}
template <class A> std::vector<A> filter (bool (*pred) (A), std::vector<A> list) {
	boost::function1<bool,A> f = pred;
	return filter (f, list);
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
template <class A> std::vector<A> replicate (unsigned n, A item) {
	std::vector<A> list;
	for (unsigned i = 0; i < n; i++)
		list.push_back (item);
	return list;
}

/** Run action N times collecting results */
template <class A> std::vector<A> repeat (unsigned n, boost::function0<A> action) {
	std::vector<A> list;
	for (unsigned i = 0; i < n; i++)
		list.push_back (action());
	return list;
}
template <class A> std::vector<A> repeat (unsigned n, A (*action) ()) {
	boost::function0<A> f = action;
	return repeat (n, f);
}

/** pair elements from each list together, final list size equals size of smaller list, remaining elements in larger list are ignored */
template <class A, class B> std::vector< std::pair<A,B> > zip (std::vector<A> as, std::vector<B> bs) {
	unsigned n = as.size() < bs.size ? as.size() : bs.size();
	std::vector< std::pair<A,B> > pairs;
	for (unsigned i = 0; i < n; i++)
		pairs.push_back (std::make_pair (as[i], bs[i]));
	return pairs;
}

template <class A> std::vector<A> intersperse (A sep, std::vector<A> list) {
	if (list.empty()) return list;
	std::vector<A> newList;
	newList.push_back (list[0]);
	for (unsigned i = 1; i < list.size(); i++) {
		newList.push_back (sep);
		newList.push_back (list[i]);
	}
	return newList;
}

inline std::string concat (std::vector<std::string> strings) {
	std::stringstream ss;
	for (unsigned i = 0; i < strings.size(); i++)
		ss << strings[i];
	return ss.str();
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

template <class T> std::string to_string (const T& t) {
	std::stringstream ss;
	ss << t;
	return ss.str();
}

template <class T> T parse_string (std::string str) {
	std::istringstream ss;
	T a;
	ss >> a;
	return a;
}

#endif /* UTIL_H_ */
