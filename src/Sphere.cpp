#include "Sphere.h"

Sphere::Sphere(glm::dvec3 const& center, glm::dvec3 const& color, double r, BRDF const& brdf) :
	Intersectable{ color, brdf }, _center { center }, _radius{ r } {}

double Sphere::rayIntersection(Ray const& ray) const
{
	glm::dvec3 rayDir{ std::move(ray.rayDirection()) };
	glm::dvec3 o_c{ ray.startPoint - _center };
	
	double a{ 1.0 }; //Dot product of ray direction with itself -> glm::dot(rayDir, rayDir) 
	double b{ glm::dot(2.0* rayDir, o_c) };
	double c{ glm::dot(o_c, o_c) - _radius * _radius };
	
	double underSqrRoot{ (b / 2.0) * (b / 2.0) - a * c };

	//Imaginary root, no solution
	if (underSqrRoot < EPS)
		return NOT_FOUND;

	double sqrRoot{ std::sqrt(underSqrRoot) };
	double intersect1{ -b / 2.0 + sqrRoot };
	double intersect2{ -b / 2.0 - sqrRoot };

	double small{ std::min(intersect1, intersect2) };
	double big{ std::max(intersect1, intersect2) };

	if (small < EPS)
		small = big;

	//No intersection with any point
	if (small < EPS)
		return NOT_FOUND;

	return small;

	//if ((small < EPS) && (big < EPS))
	//	return NOT_FOUND;
	//
	////Check if closest intersection point is inside the sphere, then we return the second one.
	//if (small < EPS)
	//	return big;

	//return small;
}

glm::dvec3 Sphere::getNormal(glm::dvec3 const& intersection) const
{
	return glm::normalize(intersection - _center);
}
