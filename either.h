/* Same as boost::variant of two types except printing should work */

#pragma once

#include <boost/optional.hpp>
#include <boost/serialization/split_free.hpp>

template <class A, class B> class Either {
public: // private to this file
	bool isRight;
	// left of right is set (according to above bool), not both
	A left;
	B right;
// public
	Either (A left) : isRight(false), left(left) {}
	Either (B right) : isRight(true), right(right) {}
	Either () {} // for serialization

	/** If Left return ptr to left, otherwise null ptr */
	boost::optional<A> mLeft () {
		return isRight ? boost::optional<A>() : boost::optional<A> (left);
	}

	/** If Right return ptr to right, otherwise null ptr */
	boost::optional<B> mRight () {
		return isRight ? boost::optional<B> (right) : boost::optional<B>();
	}
};

template <class B, class A> Either<A,B> Left (A left) {
	Either<A,B> x;
	x.isRight = false;
	x.left = left;
	return x;
}
template <class A, class B> Either<A,B> Right (B right) {
	Either<A,B> x;
	x.isRight = true;
	x.right = right;
	return x;
}

/* Printing & Serialization */

template <class A, class B> std::ostream& operator<< (std::ostream& out, const Either<A,B> &x) {
	if (x.isRight) out << "Right " << x.right; else out << "Left " << x.left;
	return out;
}

template <class A, class B> std::istream& operator>> (std::istream& in, Either<A,B> &x) {
	std::string cons;
	in >> cons;
	x.isRight = cons == "Right";
	in.ignore (1); // skip space
	if (x.isRight) in >> x.right;
	else in >> x.left;
}

namespace boost {namespace serialization {

template <class Archive, class A, class B> void save (Archive &ar, const Either<A,B> &x, const unsigned version) {
	ar << x.isRight;
	if (x.isRight) ar << x.right; else ar << x.left;
}
template <class Archive, class A, class B> void load (Archive &ar, Either<A,B> &x, const unsigned version) {
	ar >> x.isRight;
	if (x.isRight) ar >> x.right;
	else ar >> x.left;
}
template <class Archive, class A, class B> void serialize (Archive &ar, Either<A,B> &x, const unsigned version) {
	split_free (ar, x, version);
}

}}
