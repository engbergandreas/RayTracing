//#include <EasyBMP.hpp>
#ifndef __RAY_H__
#define __RAY_H__

#include <glm/glm.hpp>
#include "Intersectable.h"

class Ray {
public:
    Ray(glm::dvec3 const& start, glm::dvec3 const& dir, glm::dvec3 const& importance_);

    glm::dvec3 rayDirection(bool normalize = true) const;

    glm::dvec3 startPoint;
    glm::dvec3 direction;
    glm::dvec3 intersectionPoint;
    glm::dvec3 importance;

    Intersectable const* hitinfo;

private:
};
#endif // RAY_H