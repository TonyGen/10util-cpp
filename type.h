/* Reification of types */

#pragma once

#include <boost/units/detail/utility.hpp>

typedef std::string TypeName;

template <class T> TypeName typeName () {
	return boost::units::detail::demangle (typeid(T).name());
}

template <class T> TypeName typeName (const T &t) {
	return boost::units::detail::demangle (typeid(t).name());
}

template <template <typename> class A> TypeName typeName () {
	std::string s = boost::units::detail::demangle (typeid(A<int>).name());
	return s.substr (0, s.size() - 5);
}

template <template <typename, typename> class A> TypeName typeName () {
	std::string s = boost::units::detail::demangle (typeid(A<int, int>).name());
	return s.substr (0, s.size() - 10);
}
