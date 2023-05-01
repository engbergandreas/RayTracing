#ifndef RAYTREE_H_
#define RAYTREE_H_

#include "Ray.h"
#include "Scene.h"
#include <glm/glm.hpp>

class Scene;

class RayTree {
public:
	RayTree(Scene const& scene, Ray& ray, int maxDepth) : _root{ new Node{ ray } }, _scene{ scene }, _maxDepth{ maxDepth } {}
	~RayTree() { delete _root; };

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
	glm::dvec3 computeRayColor(Node* ptr);
	glm::dvec3 shootShadowRay(Node* ptr);
	Node* computeReflectedRay(Ray const& inRay);
	Node* computeRefractedRay(Ray const& inRay);

	Node* _root;
	Scene const& _scene;
	int _maxDepth;
};
#endif // !RAYTREE_H_
