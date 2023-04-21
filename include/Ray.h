//#include <EasyBMP.hpp>
#ifndef __RAY_H__
#define __RAY_H__
#include <glm/glm.hpp>

class Ray {
public:
    Ray(glm::dvec3 const& start, glm::dvec3 const& end, glm::dvec3 const& color);

    glm::dvec3 startPoint;
    glm::dvec3 endPoint;

    glm::dvec3 rayColor;

private:
};
#endif // RAY_H