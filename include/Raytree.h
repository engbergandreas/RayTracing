#ifndef RAYTREE_H_
#define RAYTREE_H_

#include "Ray.h"
#include "Scene.h"
#include <glm/glm.hpp>
#include <random>

class Scene;

class RayTree {
public:
	RayTree(Scene const& scene, Ray& ray, int maxDepth) :
		_root{ new Node{ ray } }, _scene{ scene }, _maxDepth{ maxDepth } {
		std::random_device rd; //Used as seed for generator, random device is a non-deterministic number generator
		
		_generator = std::mt19937(rd()); //Standard mersenne_twister_engine seeded with rd()
		_theta_distribution = std::uniform_real_distribution<>{ 0.0, 1.0 };
		_rho_distribution = std::uniform_real_distribution<>{ 0.0, 1.0 };
	}
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
	Node* computeDiffiuseReflection(Ray const& inRay);

	glm::dmat4 worldToLocal(Ray const& inRay) const;
	glm::dmat4 localToWorld(Ray const& inRay) const;
	glm::dvec3 SphericalToCartesian(double phi, double theta) const;


	Node* _root;
	Scene const& _scene;
	int _maxDepth;

	std::uniform_real_distribution<> _theta_distribution;
	std::uniform_real_distribution<> _rho_distribution;
	std::mt19937 _generator;
};
#endif // !RAYTREE_H_
