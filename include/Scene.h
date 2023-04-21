#ifndef SCENE_H_
#define SCENE_H_

#include <vector>
#include "Triangle.h"
#include "Ray.h"

//Forward decl
class Triangle;

class Scene
{
public:
	Scene();

	void shootRayIntoScene(Ray& ray) const;

private:
	std::vector<Triangle> triangles;
};
#endif // !SCENE_H_
