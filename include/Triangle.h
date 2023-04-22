#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include <glm/glm.hpp>
#include "Ray.h"
#include "Camera.h"

class Ray;

class Triangle : public Intersectable {
public:
	Triangle(glm::dvec3 const& v1, glm::dvec3 const& v2, glm::dvec3 const& v3, glm::dvec3 const& color);
	
	double rayIntersection(Ray const& ray) const override;

	glm::dvec3 const& normal() const;
	glm::dvec3 const& color() const;

	bool confirmNormalDirection(glm::dvec3 const& direction);
	void transformTriangle(glm::dmat4x4 const& M);
private:

	glm::dvec3 _v1, _v2, _v3;
	glm::dvec3 _normal;
};
#endif // __TRIANGLE_H__