#ifndef LIGHTSOURCE_H_
#define LIGHTSOURCE_H_

#include <glm/glm.hpp>
#include "Triangle.h"
#include "BRDF.h"
#include <vector>

class Triangle;

class Lightsource {
public:
	Lightsource(glm::dvec3 const& pos, glm::dvec3 const& l0, double size);

	glm::dvec3 getRandomPointOnSurface(double c1, double c2) const;
	double sideLength() const;
	glm::dvec3 normal(glm::dvec3 const&) const;

	glm::dvec3 L0() const;
	std::vector<Triangle> const& surface() const;

private:
	glm::dvec3 _position;
	glm::dvec3 _L0;
	double _sideLength;

	std::vector<Triangle> _surface;
	std::vector<glm::dvec3> _cornerPoints;
};
#endif // !LIGHTSOURCE_H_
