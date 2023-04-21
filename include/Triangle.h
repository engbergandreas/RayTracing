#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include <glm/glm.hpp>
#include "Ray.h"
#include "Camera.h"

double const EPS = 1e-7;

class Triangle {
public:
	Triangle(glm::dvec3 const& v1, glm::dvec3 const& v2, glm::dvec3 const& v3, glm::dvec3 const& color);
	
	double rayIntersection(Ray const& ray) const;

	glm::dvec3 const& normal() const;
	glm::dvec3 const& color() const;

	bool confirmNormalDirection(glm::dvec3 const& direction);

private:

	glm::dvec3 _v1, _v2, _v3;
	glm::dvec3 _color;
	glm::dvec3 _normal;
	// EasyBMP::Image img;
};
#endif // __TRIANGLE_H__