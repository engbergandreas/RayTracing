#include "Lightsource.h"

Lightsource::Lightsource(glm::dvec3 const& pos, glm::dvec3 const& l0, double size) :
	position{ pos }, L0{ l0 }, cross_section{ size } {}
