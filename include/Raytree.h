#ifndef RAYTREE_H_
#define RAYTREE_H_

#include "Ray.h"
#include "Scene.h"
#include <glm/glm.hpp>
#include <random>
#include "Utils.h"

class Scene;

class RayTree {
public:
	RayTree(Scene const& scene, Ray& ray);
	~RayTree() { delete _root; }; //Rest of tree is deleted recursively by Node class

	void createRayTree();
	glm::dvec3 computeRadiance();

private:
	class Node {
	public:
		Node(Ray const& ray, Node* parent = nullptr, Node* reflected = nullptr, Node* refracted = nullptr) : 
			_ray{ ray }, _parent{ parent }, _reflected{ reflected }, _refracted{ refracted } {}
		~Node() { if (_reflected) delete _reflected; if (_refracted) delete _refracted; };

		Node* _parent;
		Node* _reflected;
		Node* _refracted;

		Ray _ray;
	};//Node

	void buildRayTree(Node* ptr, int depth);
	bool terminateRayPath(Node* ptr, int depth) const;
	glm::dvec3 computeRayColor(Node* ptr);
	glm::dvec3 shootShadowRay(Node* ptr);
	bool isShadowRayVisible(Ray const& shadowRay) const;

	Node* computeReflectedRay(Ray const& inRay);
	Node* computeRefractedRay(Ray const& inRay);
	Node* computeDiffiuseReflection(Ray const& inRay);
	bool russianRoulette(Ray const& inRay, double randomValue) const;

	glm::dmat4 worldToLocal(Ray const& inRay) const;
	glm::dmat4 localToWorld(Ray const& inRay) const;
	glm::dvec3 SphericalToCartesian(double phi, double theta) const;

	Node* _root;
	Scene const& _scene;
};
#endif // !RAYTREE_H_
