
#include "module.h"

module::Module module::Module::operator+ (const Module &mod) const {
	return concat (items (*this, mod));
}

module::Module module::concat (std::vector<Module> mods) {
	Module newMod;
	for (unsigned i = 0; i < mods.size(); i++) {
		push_all (newMod.libPaths, mods[i].libPaths);
		push_all (newMod.includePaths, mods[i].includePaths);
		push_all (newMod.libNames, mods[i].libNames);
		push_all (newMod.includeFiles, mods[i].includeFiles);
	}
	return newMod;
}

template <> module::Module type<int>::module = module::Module();
template <> module::Module type<void>::module = module::Module();


std::ostream& operator<< (std::ostream& out, const module::Module &x) {
	out << "Module " << x.libPaths << ", " << x.includePaths << ", " << x.libNames << ", " << x.includeFiles;
	return out;
}
