#include "Triangle.h"

Triangle::Triangle(glm::dvec3 const& v1, glm::dvec3 const& v2, glm::dvec3 const& v3, glm::dvec3 const& color) : 
	Intersectable{ color }, _v1 { v1 }, _v2{v2}, _v3{v3}
{
	glm::dvec3 e1{ _v2 - _v1 };
	glm::dvec3 e2{ _v3 - _v1 };
	_normal = glm::normalize(glm::cross(e1, e2));
}

double Triangle::rayIntersection(Ray const& ray) const
{
	glm::dvec3 T{ ray.startPoint - _v1 };
	glm::dvec3 E1{ _v2 - _v1 };
	glm::dvec3 E2{ _v3 - _v1 };
	glm::dvec3 D{ ray.endPoint - ray.startPoint };

	//Ray points in the same direction as normal, i.e., can't hit the triangle.
	double dprod{ glm::dot(glm::normalize(D), _normal) };
	if (dprod > 0)  //or -EPS;
		return NOT_FOUND;

	glm::dvec3 P{ glm::cross(D, E2) };

	double PE1{ glm::dot(P, E1) };

	//ray parallel to triangle
	if (PE1 > -EPS && PE1 < EPS)
		return NOT_FOUND;

	const double PE1_inv{ 1.0 / PE1 };

	const double u{ PE1_inv * glm::dot(P,T) };
	//ray intersection outside triangle
	if (u < -EPS || u > 1.0)
		return NOT_FOUND;

	glm::dvec3 Q{ glm::cross(T, E1) };

	const double v{ PE1_inv * glm::dot(Q, D) };
	//ray intersection outside triangle
	if (v < -EPS || (u + v) > 1.0)
		return NOT_FOUND;

	double t{ PE1_inv * glm::dot(Q, E2) };

	if (t > EPS)
		return t; //intersection infront of ray origin

	return NOT_FOUND; //intersection behind ray origin
}


glm::dvec3 const& Triangle::normal() const
{
	return _normal;
}

glm::dvec3 const& Triangle::color() const
{
	return _color;
}

bool Triangle::confirmNormalDirection(glm::dvec3 const& d)
{
	return (std::abs(_normal.x - d.x) < EPS) &&
		(std::abs(_normal.y - d.y) < EPS) &&
		(std::abs(_normal.z - d.z) < EPS);
}

void Triangle::transformTriangle(glm::dmat4x4 const& M)
{
	_v1 = M * glm::dvec4(_v1, 1.0);
	_v2 = M * glm::dvec4(_v2, 1.0);
	_v3 = M * glm::dvec4(_v3, 1.0);
}
