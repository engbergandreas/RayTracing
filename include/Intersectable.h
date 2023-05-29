#ifndef INTERSECTABLE_H_
#define INTERSECTABLE_H_

#include "Ray.h"
#include <glm/glm.hpp>
#include "BRDF.h"
#include "Utils.h"

class Ray;

double const EPS{ constants::EPSILON };
const double NOT_FOUND{ constants::NOT_FOUND };

class Intersectable {
public:
	virtual ~Intersectable() = default;
	virtual double rayIntersection(Ray const& ray) const = 0;
	virtual glm::dvec3 getNormal(glm::dvec3 const& intersection) const = 0;
	glm::dvec3 const& color() const { return _color; };
	BRDF const& brdf() const { return _brdf; };

protected: 
	Intersectable(glm::dvec3 const& color, BRDF const& brdf) : _color{ color }, _brdf{ brdf } {}
	glm::dvec3 _color;
	BRDF _brdf;
};
#endif // !INTERSECTABLE_H_
