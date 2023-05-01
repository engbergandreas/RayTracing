#ifndef TETRAHEDRON_H_
#define TETRAHEDRON_H_

#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Triangle.h"

class Triangle;

class Tetrahedron {
public:
	Tetrahedron(glm::dvec3 const& color, BRDF const& brdf);

	//double rayIntersection(Ray& ray, double t) const override;

	void transformObject(glm::dmat4x4 const& M);

	std::vector<Triangle> sides;
private:

};
#endif // !TETRAHEDRON_H_
