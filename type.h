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
