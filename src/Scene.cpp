#include "Scene.h"

Scene::Scene() {
	//Build room of triangles are defined as looking towards them from inside them room.
	//Top half
	Triangle* topWallLeft{ new Triangle{ {0.0, 6.0, -5.0}, {10.0, 6.0, 5.0}, {0.0, 6.0, 5.0 }, { 1.0, 0.0, 0.0 } } }; //red 
	Triangle* topWallRight{ new Triangle{ {0.0, 6.0, -5.0}, {10.0, 6.0, -5.0}, {10.0, 6.0, 5.0 }, { 1.0, 0.0, 0.0 } } }; //red 
	Triangle* topWallLeftLeft{ new Triangle{ {-3.0, 0.0, -5.0}, {0.0, 6.0, 5.0}, {-3.0, 0.0, 5.0 }, { 0.0, 0.0, 1.0 } } }; //blue 
	Triangle* topWallLeftRight{ new Triangle{ {-3.0, 0.0, -5.0}, {0.0, 6.0, -5.0}, {0.0, 6.0, 5.0 }, { 0.0, 0.0, 1.0 } } }; //blue 
	Triangle* topWallRightLeft{ new Triangle{ {10.0, 6.0, -5.0}, {13.0, 0.0, 5.0}, {10.0, 6.0, 5.0 }, { 1.0, 1.0, 0.0 } } }; //red-green 
	Triangle* topWallRightRight{ new Triangle{ {10.0, 6.0, -5.0}, {13.0, 0.0, -5.0}, {13.0, 0.0, 5.0 }, { 1.0, 1.0, 0.0 } } }; //red-green
	//Bottom half
	Triangle* bottomWallLeft{ new Triangle{ {10.0, -6.0, -5.0}, {0.0, -6.0, 5.0}, {10.0, -6.0, 5.0 }, { 0.0, 1.0, 0.0 } } }; //green
	Triangle* bottomWallRight{ new Triangle{ {10.0, -6.0, -5.0}, {0.0, -6.0, -5.0}, {0.0, -6.0, 5.0 }, { 0.0, 1.0, 0.0 } } }; //green
	Triangle* bottomWallLeftLeft{ new Triangle{ {.0, -6.0, -5.0}, {-3.0, 0.0, 5.0}, {0.0, -6.0, 5.0 }, { 1.0, 0.0, 1.0 } } }; //purple
	Triangle* bottomWallLeftRight{ new Triangle{ {0.0, -6.0, -5.0}, {-3.0, 0.0, -5.0}, {-3.0, 0.0, 5.0 }, { 1.0, 0.0, 1.0 } } }; //purple
	Triangle* bottomWallRightLeft{ new Triangle{ {13.0, 0.0, -5.0}, {10.0, -6.0, 5.0}, {13.0, 0.0, 5.0 }, { 0.0, 1.0, 1.0 } } }; //yewllo
	Triangle* bottomWallRightRight{ new Triangle{ {13.0, 0.0, -5.0}, {10.0, -6.0, -5.0}, {10.0, -6.0, 5.0 }, { 0.0, 1.0, 1.0 } } }; //yewllo
	//Ceiling
	Triangle* cLeft{ new Triangle{ {-3.0, 0.0, 5.0}, {0.0, 6.0, 5.0}, {0.0, -6.0, 5.0}, {1.0, 1.0, 1.0} } };
	Triangle* cMLeft{ new Triangle{ {0.0, -6.0, 5.0}, {0.0, 6.0, 5.0}, {10.0, 6.0, 5.0}, {1.0, 1.0, 1.0} } };
	Triangle* cMright{ new Triangle{ {0.0, -6.0, 5.0}, {10.0, 6.0, 5.0}, {10.0, -6.0, 5.0}, {1.0, 1.0, 1.0} } };
	Triangle* cRight{ new Triangle{ {13.0, 0.0, 5.0}, {10.0, -6.0, 5.0}, {10.0, 6.0, 5.0}, {1.0, 1.0, 1.0} } };
	//Floor
	Triangle* fLeft{ new Triangle{ {-3.0, 0.0, -5.0}, {0.0, -6.0, -5.0}, {0.0, 6.0, -5.0}, {1.0, 1.0, 1.0} } };
	Triangle* fMLeft{ new Triangle{ {0.0, -6.0, -5.0}, {10.0, 6.0, -5.0}, {0.0, 6.0, -5.0}, {1.0, 1.0, 1.0} } };
	Triangle* fMright{ new Triangle{ {0.0, -6.0, -5.0}, {10.0, -6.0, -5.0}, {10.0, 6.0, -5.0}, {1.0, 1.0, 1.0} } };
	Triangle* fRight{ new Triangle{ {13.0, 0.0, -5.0}, {10.0, 6.0, -5.0}, {10.0, -6.0, -5.0}, {1.0, 1.0, 1.0} } };

	objects.push_back(topWallLeft);
	objects.push_back(topWallRight);
	objects.push_back(topWallLeftLeft);
	objects.push_back(topWallLeftRight);
	objects.push_back(topWallRightLeft);
	objects.push_back(topWallRightRight);
	objects.push_back(bottomWallLeft);
	objects.push_back(bottomWallRight);
	objects.push_back(bottomWallLeftLeft);
	objects.push_back(bottomWallLeftRight);
	objects.push_back(bottomWallRightLeft);
	objects.push_back(bottomWallRightRight);
	objects.push_back(cLeft);
	objects.push_back(cMLeft);
	objects.push_back(cMright);
	objects.push_back(cRight);
	objects.push_back(fLeft);
	objects.push_back(fMLeft);
	objects.push_back(fMright);
	objects.push_back(fRight);

	//assert(topWallLeft.confirmNormalDirection({ 0.0, -1.0, 0.0 }));
	assert(topWallRight->confirmNormalDirection({ 0.0, -1.0, 0.0 }));
	assert(bottomWallLeft->confirmNormalDirection({ 0.0, 1.0, 0.0 }));
	assert(bottomWallRight->confirmNormalDirection({ 0.0, 1.0, 0.0 }));
	//Ceiling
	assert(cLeft->confirmNormalDirection({ 0.0, 0.0, -1.0 }));
	assert(cMLeft->confirmNormalDirection({ 0.0, 0.0, -1.0 }));
	assert(cMright->confirmNormalDirection({ 0.0, 0.0, -1.0 }));
	assert(cRight->confirmNormalDirection({ 0.0, 0.0, -1.0 }));
	//Floor
	assert(fLeft->confirmNormalDirection({ 0.0, 0.0, 1.0 }));
	assert(fMLeft->confirmNormalDirection({ 0.0, 0.0, 1.0 }));
	assert(fMright->confirmNormalDirection({ 0.0, 0.0, 1.0 }));
	assert(fRight->confirmNormalDirection({ 0.0, 0.0, 1.0 }));

	//Add other objects to the scene
	Tetrahedron tetra{ glm::dvec3{0.52, 0.32, 0.87} };
	glm::dmat4 S{ glm::scale(glm::dmat4{ 1.0 }, glm::dvec3{ 2.0, 2.0, 2.0 }) };
	glm::dmat4 R{ glm::rotate(glm::dmat4{1.0}, glm::radians(-35.0), glm::dvec3{0.0, 0.0, 1.0}) };
	glm::dmat4 R2{ glm::rotate(glm::dmat4{1.0}, glm::radians(30.0), glm::dvec3{0.0, 1.0, 0.0})};
	glm::dmat4 T{ glm::translate(glm::dmat4{1.0}, glm::dvec3{4.0, 3.0, -1.0}) };
	tetra.transformObject(S);
	tetra.transformObject(R2);
	tetra.transformObject(R);
	tetra.transformObject(T);

	for (auto&& triangle : tetra.sides) {
		objects.push_back(new Triangle{ std::move(triangle) });
	}
	
	Sphere* sphere{ new Sphere{glm::dvec3{5.0, -2.0, -2.0}, glm::dvec3{0.7, 0.2, 0.4}, 1.4} };
	objects.push_back(sphere);

	//Add light sources
	Lightsource sphericalLight{ glm::dvec3{5.0, 0.0, 5.0}, {1.0, 1.0, 1.0}, 0.5 };
	lightSources.push_back(sphericalLight);
}

Scene::~Scene() {
	for (Intersectable* obj : objects) {
		delete obj;
	}
}
void Scene::shootRayIntoScene(Ray& ray) const
{
	//Store closest object hit in ray 
	double closest_t{ std::numeric_limits<double>::max() };
	for (Intersectable const*  obj : objects) {
		double t = obj->rayIntersection(ray);
		if (t > 0 && t < closest_t) {
			closest_t = t;
			ray.hitinfo = obj;
		}
	}

	if (!ray.hitinfo)
		return;

	Lightsource light{ lightSources.back() };
	glm::dvec3 intersectionPoint{ ray.startPoint + closest_t * glm::normalize(ray.endPoint - ray.startPoint) };
	glm::dvec3 endPoint{ light.position };

	Ray shadowRay{ intersectionPoint, endPoint, {0.0, 0.0, 0.0} };

	//Check if shadow ray is intersecting another object before reaching light source
	for (Intersectable const*  obj : objects) {
		double t = obj->rayIntersection(shadowRay);
		if (t > 0.0 && t < (1.0 - EPS)) {
			ray.rayColor = { 0.0, 0.0, 0.0 };
			return;
		}
	}

	double r{ glm::length(shadowRay.rayDirection(false)) };

	double cosTheta{ glm::dot(shadowRay.rayDirection(), ray.hitinfo->getNormal(intersectionPoint)) };
	cosTheta = std::max(cosTheta, 0.0);

	double Sigma{ light.cross_section / (r * r) };
	glm::dvec3 irradiance{ Sigma * cosTheta * light.L0 };
	
	glm::dvec3 rho{ 1.0, 1.0, 1.0 };

 	glm::dvec3 emittedLight{ rho * irradiance / 3.14159 };

	ray.rayColor = ray.hitinfo->color() * emittedLight;

	
	//ray.rayColor = ray.hitinfo->color();
	
}



