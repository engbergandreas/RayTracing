#include "Lightsource.h"

Lightsource::Lightsource(glm::dvec3 const& pos, glm::dvec3 const& l0, double sideLength) :
	_position{ pos }, _L0{ l0 }, _sideLength{ sideLength } 
{	
	//Create a quad centered on origo with side length 
	//Assumes a ceiling area lightsource
	double halfLength = sideLength / 2.0;
	glm::dvec3 v0{ -halfLength, -halfLength, 0.0 };
	glm::dvec3 v1{  halfLength, -halfLength, 0.0 };
	glm::dvec3 v2{  halfLength,  halfLength, 0.0 };
	glm::dvec3 v3{ -halfLength,  halfLength, 0.0 };
	//Translate points
	glm::dmat4 T{ glm::translate(glm::dmat4{ 1.0 }, pos) };
	v0 = T * glm::dvec4{ v0, 1.0 };
	v1 = T * glm::dvec4{ v1, 1.0 };
	v2 = T * glm::dvec4{ v2, 1.0 };
	v3 = T * glm::dvec4{ v3, 1.0 };

	BRDF brdf{ Material::lightsource, glm::dvec3{ 1.0 }, 1.0 };
	Triangle t1{ v0, v2, v1 , glm::dvec3{ _L0 }, brdf};
	Triangle t2{ v0, v3, v2, glm::dvec3{ _L0 }, brdf };

	_surface.push_back(t1);
	_surface.push_back(t2);

	_cornerPoints.push_back(v0);
	_cornerPoints.push_back(v1);
	_cornerPoints.push_back(v2);
	_cornerPoints.push_back(v3);
}

glm::dvec3 Lightsource::getRandomPointOnSurface(double c1, double c2) const
{
	//Random point on the lightsource is given by 
	//X_L(c1,c2) = v0 + c1*e1 + c2*e2 where 0<=c1,c2<=sidelength
	glm::dvec3 e1{ glm::normalize(_cornerPoints[1] - _cornerPoints[0]) };
	glm::dvec3 e2{ glm::normalize(_cornerPoints[2] - _cornerPoints[0]) };
	return _cornerPoints[0] + c1 * e1 + c2 * e2;
}
double Lightsource::sideLength() const {
	return _sideLength;
}

glm::dvec3 Lightsource::normal(glm::dvec3 const& p) const {
	return _surface[0].getNormal(p);
}

glm::dvec3 Lightsource::L0() const {
	return _L0;
}
std::vector<Triangle> const& Lightsource::surface() const {
	return _surface;
}