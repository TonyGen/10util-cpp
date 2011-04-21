/* Unit type. See http://en.wikipedia.org/wiki/Unit_type */

#ifndef UNIT_H_
#define UNIT_H_

/** Unit type is like void but its single value can be passed between functions; useful for generic functions */
struct Unit {
	friend std::ostream& operator<< (std::ostream& out, const Unit& x) {out << "unit"; return out;}
};

const Unit unit = Unit();

#endif /* UNIT_H_ */
