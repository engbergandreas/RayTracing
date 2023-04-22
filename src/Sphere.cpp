#include "Sphere.h"

Sphere::Sphere(glm::dvec3 const& center, glm::dvec3 const& color, double r) :
	Intersectable{ color }, _center { center }, _radius{ r } {}

double Sphere::rayIntersection(Ray const& ray) const
{
	glm::dvec3 rayDir{ std::move(ray.rayDirection()) };
	glm::dvec3 o_c{ ray.startPoint - _center };
	
	double a{ glm::dot(rayDir, rayDir) };
	double b{ 2 * glm::dot(rayDir, o_c) };
	double c{ glm::dot(o_c, o_c) - _radius * _radius };
	
	double underSqrRoot{ (b / 2.0) * (b / 2.0) - a * c };

	//Imaginary root, no solution
	if (underSqrRoot < EPS)
		return NOT_FOUND;

	double sqrRoot{ std::sqrt(underSqrRoot) };
	double intersect1{ -b / 2.0 + sqrRoot };
	double intersect2{ -b / 2.0 - sqrRoot };

	//Return the closest intersection point
	return std::min(intersect1, intersect2);
}
