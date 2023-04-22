#ifndef SCENE_H_
#define SCENE_H_

#include <vector>
#include "Ray.h"
#include "Intersectable.h"
#include "Triangle.h"
#include "Tetrahedron.h"
#include "Sphere.h"

class Scene
{
public:
	Scene();
	~Scene();

	void shootRayIntoScene(Ray& ray) const;

private:
	std::vector<Intersectable*> objects;
};
#endif // !SCENE_H_
