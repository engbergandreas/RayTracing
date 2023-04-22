#include "Ray.h"

Ray::Ray(glm::dvec3 const& start, glm::dvec3 const& end, glm::dvec3 const& color) :
	startPoint{ start }, endPoint{ end }, rayColor{ color }, hitinfo{ nullptr } {}

glm::dvec3 Ray::rayDirection(bool normalize) const
{
	if (normalize)
		return glm::normalize(endPoint - startPoint);
	
	return endPoint - startPoint;
}
