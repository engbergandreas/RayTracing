#ifndef OBJECT_H_
#define OBJECT_H_

#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Triangle.h"

class Triangle;

//Wrapper class for objects that consists of multiple triangles. 
class Object {
public:
	Object() = default;
    virtual ~Object() = default;

	void transformObject(glm::dmat4x4 const& M);

	std::vector<Triangle> sides;
};

class Tetrahedron : public Object {
public:
	Tetrahedron(glm::dvec3 const& color, BRDF const& brdf);
	~Tetrahedron() = default;
};

class Cube : public Object {
public:
	Cube(glm::dvec3 const& color, BRDF const& brdf);
	~Cube() = default;
};
#endif // !OBJECT_H_
