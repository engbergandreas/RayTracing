#include "Tetrahedron.h"

Tetrahedron::Tetrahedron(glm::dvec3 const& color, BRDF const& brdf)
{
	Triangle t0(glm::dvec3{ 0.0, 0.0, 0.0 }, glm::dvec3{ 0.0, 1.0, 0.0 }, glm::dvec3{ 1.0, 0.0, 0.0 }, color, brdf);
	Triangle t1(glm::dvec3{ 0.0, 0.0, 0.0 }, glm::dvec3{ 0.5, 0.5, 1.0 }, glm::dvec3{ 0.0, 1.0, 0.0 }, color, brdf);
	Triangle t2(glm::dvec3{ 1.0, 0.0, 0.0 }, glm::dvec3{ 0.0, 1.0, 0.0 }, glm::dvec3{ 0.5, 0.5, 1.0 }, color, brdf);
	Triangle t3(glm::dvec3{ 0.0, 0.0, 0.0 }, glm::dvec3{ 1.0, 0.0, 0.0 }, glm::dvec3{ 0.5, 0.5, 1.0 }, color, brdf);
	
	sides.push_back(t0);
	sides.push_back(t1);
	sides.push_back(t2);
	sides.push_back(t3);
}

void Tetrahedron::transformObject(glm::dmat4x4 const& M)
{
	for (Triangle& t : sides) {
		t.transformTriangle(M);
	}
}
