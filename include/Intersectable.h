#ifndef INTERSECTABLE_H_
#define INTERSECTABLE_H_

#include "Ray.h"
#include <glm/glm.hpp>

class Ray;

double const EPS = 1e-7;
const double NOT_FOUND = -1.0;

class Intersectable {
public:
	virtual ~Intersectable() = default;
	virtual double rayIntersection(Ray const& ray) const = 0;

	glm::dvec3 const& color() const { return _color; };

protected: 
	Intersectable(glm::dvec3 const& color) : _color{ color } {}
	glm::dvec3 _color;
	//Possibly add get normal function aswell.
};
#endif // !INTERSECTABLE_H_
