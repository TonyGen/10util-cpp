/* Unit type. See http://en.wikipedia.org/wiki/Unit_type */

#pragma once

#include <ostream>

/** Unit type is like void but its single value can be passed between functions; useful for generic functions */
struct Unit {};

const Unit unit = Unit();

inline std::ostream& operator<< (std::ostream& out, const Unit &x) {out << "unit"; return out;}

namespace boost {namespace serialization {

template <class Archive> void serialize (Archive & ar, Unit &x, const unsigned version) {}

}}
