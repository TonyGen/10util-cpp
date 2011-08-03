/* Helper functions for map */

#pragma once

#include <map>
#include <boost/optional.hpp>
#include <vector>

namespace umap {

/** Remove entry at given key and return its value if it was present */
template <class K, class V> boost::optional<V> remove (std::map<K,V> &map, K key) {
	typename std::map<K,V>::iterator it = map.find (key);
	if (it) {
		V value = it->second;
		map.erase (it);
		return boost::optional<V>(value);
	} else
		return boost::optional<V>();
}

template <class K, class V> std::vector<K> keys (std::map<K,V> &map) {
	std::vector<V> list;
	for (typename std::map<K,V>::iterator it = map.begin(); it != map.end(); ++it)
		list.push_back (it->first);
	return list;
}

template <class K, class V> std::vector<V> values (std::map<K,V> &map) {
	std::vector<V> list;
	for (typename std::map<K,V>::iterator it = map.begin(); it != map.end(); ++it)
		list.push_back (it->second);
	return list;
}

}
