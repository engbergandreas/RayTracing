#ifndef SCENE_H_
#define SCENE_H_

#include <vector>
#include "Ray.h"
#include "Intersectable.h"
#include "Triangle.h"
#include "Tetrahedron.h"
#include "Sphere.h"
#include "Lightsource.h"
#include "Utils.h"

class Scene
{
public:
	Scene();
	~Scene();

	void shootRayIntoScene(Ray& ray) const;

	std::vector<Lightsource> const& getLightSources() const;
	std::vector<Intersectable*> const& getObjects() const;

private:
	std::vector<Intersectable*> objects;
	std::vector<Lightsource> lightSources;
};
#endif // !SCENE_H_
