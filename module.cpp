
#include "module.h"
#include <map>

static std::map <module::Header, std::vector<module::Lib> > Registry;

/** Return dependent libraries of given header, ie. those that you must link with when including given header */
std::vector<module::Lib> module::libraries (Header header) {
	return Registry [header];
}

/** Register that the given header requires the user links with the given libraries when including the header */
void module::registerLibraries (Header header, std::vector<Lib> libs) {
	Registry [header] = libs;
}
