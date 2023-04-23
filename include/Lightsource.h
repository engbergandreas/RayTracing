#ifndef LIGHTSOURCE_H_
#define LIGHTSOURCE_H_

#include <glm/glm.hpp>

class Lightsource {
public:
	Lightsource(glm::dvec3 const& pos, glm::dvec3 const& l0, double size);

	glm::dvec3 position;
	glm::dvec3 L0;
	double cross_section;

};
#endif // !LIGHTSOURCE_H_
