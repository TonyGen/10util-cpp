/* Helper functions for map */

#pragma once

#include <map>
#include <boost/optional.hpp>

namespace umap {

/** Remove entry at given key and return its value if it was present */
template <class K, class V> boost::optional<V> remove (std::map<K,V> &map, K key) {
	std::map<K,V>::iterator it = map.find (key);
	if (it) {
		V value = it->second;
		map.erase (it);
		return boost::optional<V>(value);
	} else
		return boost::optional<V>();
}

}