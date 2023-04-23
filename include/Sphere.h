#ifndef SPHERE_H_
#define SPHERE_H_

#include <glm/glm.hpp>
#include "Ray.h"

class Sphere : public Intersectable {
public:
	Sphere(glm::dvec3 const& center, glm::dvec3 const& color, double r);
	~Sphere() = default;

	double rayIntersection(Ray const& ray) const override;
	glm::dvec3 getNormal(glm::dvec3 const& intersection) const override;
private:
	glm::dvec3 _center;
	double _radius;
};
#endif // !SPHERE_H_
