/* Helper functions on vectors */

#pragma once

#include <vector>
#include <sstream>
#include <boost/function.hpp>
#include <set>

template <class A> std::vector<A> fromSet (std::set<A> set) {
	std::vector<A> list;
	for (std::set<A>::iterator it = set.begin(); it != set.end(); ++it)
		list.push_back (*it);
	return list;
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

template <class A, class B, template <typename, typename> class F> std::vector<B> fmap (F<B,A> fun, std::vector<A> list) {
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

template <class A> std::vector<A> take (unsigned n, std::vector<A> items) {
	std::vector<A> list;
	unsigned m = n < items.size() ? n : items.size();
	for (unsigned i = 0; i < m; i++)
		list.push_back (items[i]);
	return list;
}

template <class A> std::vector<A> drop (unsigned n, std::vector<A> items) {
	std::vector<A> list;
	for (unsigned i = n; i < items.size(); i++)
		list.push_back (items[i]);
	return list;
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

/** Enumerate numbers from 0 to n-1 */
template <class A> std::vector<A> enumerate (A n) {
	std::vector<A> list;
	for (A i = 0; i < n; i++) list.push_back (i);
	return list;
}

/** pair elements from each list together, final list size equals size of smaller list, remaining elements in larger list are ignored */
template <class A, class B> std::vector< std::pair<A,B> > zip (std::vector<A> as, std::vector<B> bs) {
	unsigned n = as.size() < bs.size() ? as.size() : bs.size();
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

template <class A> std::vector<A> items (A a1) {
	std::vector<A> list;
	list.push_back (a1);
	return list;
}

template <class A> std::vector<A> items (A a1, A a2) {
	std::vector<A> list;
	list.push_back (a1);
	list.push_back (a2);
	return list;
}

template <class A> std::vector<A> items (A a1, A a2, A a3) {
	std::vector<A> list;
	list.push_back (a1);
	list.push_back (a2);
	list.push_back (a3);
	return list;
}

template <class A> std::vector<A> items (A a1, A a2, A a3, A a4) {
	std::vector<A> list;
	list.push_back (a1);
	list.push_back (a2);
	list.push_back (a3);
	list.push_back (a4);
	return list;
}
