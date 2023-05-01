#include "Ray.h"

Ray::Ray(glm::dvec3 const& start, glm::dvec3 const& dir, glm::dvec3 const& importance_) :
	startPoint{ start }, direction{ dir }, importance{ importance_ },
	intersectionPoint{ glm::dvec3{ 0.0, 0.0, 0.0 } }, hitinfo{ nullptr } {}

glm::dvec3 Ray::rayDirection(bool normalize) const
{
	if (normalize)
		return glm::normalize(direction);
	
	return direction;
}
