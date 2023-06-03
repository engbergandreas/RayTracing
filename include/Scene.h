#ifndef SCENE_H_
#define SCENE_H_

#include <vector>
#include "Ray.h"
#include "Intersectable.h"
#include "Triangle.h"
#include "Object.h"
#include "Sphere.h"
#include "Lightsource.h"
#include "Utils.h"

class Lightsource;
class Object;

class Scene
{
public:
	Scene();
	~Scene();

	void shootRayIntoScene(Ray& ray) const;

	std::vector<Lightsource> const& getLightSources() const;
	std::vector<Intersectable const*> const& getObjects() const;

private:
	void addLightsource(Lightsource const& source);
	void addObject(Object& source);

	std::vector<Intersectable const*> objects;
	std::vector<Lightsource> lightSources;
};
#endif // !SCENE_H_
