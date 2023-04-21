#include "Scene.h"

Scene::Scene() {
	//Build room of triangles are defined as looking towards them from inside them room.
	//Top half
	Triangle topWallLeft{ {0.0, 6.0, -5.0}, {10.0, 6.0, 5.0}, {0.0, 6.0, 5.0 }, { 1.0, 0.0, 0.0 } }; //red 
	Triangle topWallRight{ {0.0, 6.0, -5.0}, {10.0, 6.0, -5.0}, {10.0, 6.0, 5.0 }, { 1.0, 0.0, 0.0 } }; //red 
	Triangle topWallLeftLeft{ {-3.0, 0.0, -5.0}, {0.0, 6.0, 5.0}, {-3.0, 0.0, 5.0 }, { 0.0, 0.0, 1.0 } }; //blue 
	Triangle topWallLeftRight{ {-3.0, 0.0, -5.0}, {0.0, 6.0, -5.0}, {0.0, 6.0, 5.0 }, { 0.0, 0.0, 1.0 } }; //blue 
	Triangle topWallRightLeft{ {10.0, 6.0, -5.0}, {13.0, 0.0, 5.0}, {10.0, 6.0, 5.0 }, { 1.0, 1.0, 0.0 } }; //red-green 
	Triangle topWallRightRight{ {10.0, 6.0, -5.0}, {13.0, 0.0, -5.0}, {13.0, 0.0, 5.0 }, { 1.0, 1.0, 0.0 } }; //red-green
	//Bottom half
	Triangle bottomWallLeft{ {10.0, -6.0, -5.0}, {0.0, -6.0, 5.0}, {10.0, -6.0, 5.0 }, { 0.0, 1.0, 0.0 } }; //green
	Triangle bottomWallRight{ {10.0, -6.0, -5.0}, {0.0, -6.0, -5.0}, {0.0, -6.0, 5.0 }, { 0.0, 1.0, 0.0 } }; //green
	Triangle bottomWallLeftLeft{ {.0, -6.0, -5.0}, {-3.0, 0.0, 5.0}, {0.0, -6.0, 5.0 }, { 1.0, 0.0, 1.0 } }; //purple
	Triangle bottomWallLeftRight{ {0.0, -6.0, -5.0}, {-3.0, 0.0, -5.0}, {-3.0, 0.0, 5.0 }, { 1.0, 0.0, 1.0 } }; //purple
	Triangle bottomWallRightLeft{ {13.0, 0.0, -5.0}, {10.0, -6.0, 5.0}, {13.0, 0.0, 5.0 }, { 0.0, 1.0, 1.0 } }; //yewllo
	Triangle bottomWallRightRight{ {13.0, 0.0, -5.0}, {10.0, -6.0, -5.0}, {10.0, -6.0, 5.0 }, { 0.0, 1.0, 1.0 } }; //yewllo
	//Ceiling
	Triangle cLeft{ {-3.0, 0.0, 5.0}, {0.0, 6.0, 5.0}, {0.0, -6.0, 5.0}, {0.5, 1.0, 1.0} };
	Triangle cMLeft{ {0.0, -6.0, 5.0}, {0.0, 6.0, 5.0}, {10.0, 6.0, 5.0}, {0.5, 1.0, 1.0} };
	Triangle cMright{ {0.0, -6.0, 5.0}, {10.0, 6.0, 5.0}, {10.0, -6.0, 5.0}, {0.5, 1.0, 1.0} };
	Triangle cRight{ {13.0, 0.0, 5.0}, {10.0, -6.0, 5.0}, {10.0, 6.0, 5.0}, {0.5, 1.0, 1.0} };
	//Floor
	Triangle fLeft{ {-3.0, 0.0, -5.0}, {0.0, -6.0, -5.0}, {0.0, 6.0, -5.0}, {1.0, 1.0, 1.0} };
	Triangle fMLeft{ {0.0, -6.0, -5.0}, {10.0, 6.0, -5.0}, {0.0, 6.0, -5.0}, {1.0, 1.0, 1.0} };
	Triangle fMright{ {0.0, -6.0, -5.0}, {10.0, -6.0, -5.0}, {10.0, 6.0, -5.0}, {1.0, 1.0, 1.0} };
	Triangle fRight{ {13.0, 0.0, -5.0}, {10.0, 6.0, -5.0}, {10.0, -6.0, -5.0}, {1.0, 1.0, 1.0} };

	triangles.push_back(topWallLeft);
	triangles.push_back(topWallRight);
	triangles.push_back(topWallLeftLeft);
	triangles.push_back(topWallLeftRight);
	triangles.push_back(topWallRightLeft);
	triangles.push_back(topWallRightRight);
	triangles.push_back(bottomWallLeft);
	triangles.push_back(bottomWallRight);
	triangles.push_back(bottomWallLeftLeft);
	triangles.push_back(bottomWallLeftRight);
	triangles.push_back(bottomWallRightLeft);
	triangles.push_back(bottomWallRightRight);
	triangles.push_back(cLeft);
	triangles.push_back(cMLeft);
	triangles.push_back(cMright);
	triangles.push_back(cRight);
	triangles.push_back(fLeft);
	triangles.push_back(fMLeft);
	triangles.push_back(fMright);
	triangles.push_back(fRight);

	assert(topWallLeft.confirmNormalDirection({ 0.0, -1.0, 0.0 }));
	assert(topWallRight.confirmNormalDirection({ 0.0, -1.0, 0.0 }));
	assert(bottomWallLeft.confirmNormalDirection({ 0.0, 1.0, 0.0 }));
	assert(bottomWallRight.confirmNormalDirection({ 0.0, 1.0, 0.0 }));
	//Ceiling
	assert(cLeft.confirmNormalDirection({ 0.0, 0.0, -1.0 }));
	assert(cMLeft.confirmNormalDirection({ 0.0, 0.0, -1.0 }));
	assert(cMright.confirmNormalDirection({ 0.0, 0.0, -1.0 }));
	assert(cRight.confirmNormalDirection({ 0.0, 0.0, -1.0 }));
	//Floor
	assert(fLeft.confirmNormalDirection({ 0.0, 0.0, 1.0 }));
	assert(fMLeft.confirmNormalDirection({ 0.0, 0.0, 1.0 }));
	assert(fMright.confirmNormalDirection({ 0.0, 0.0, 1.0 }));
	assert(fRight.confirmNormalDirection({ 0.0, 0.0, 1.0 }));
}

void Scene::shootRayIntoScene(Ray& ray) const
{
	double t{ std::numeric_limits<double>::max() };

	for (const Triangle& triangle : triangles) {
		double tmp{ triangle.rayIntersection(ray) };

		if (tmp > 0 && tmp < t) {
			ray.rayColor = triangle.color();
			t = tmp;
			//Todo set ray intersection point 
			//and reference to the triangle if we need it. 
		}
	}
}



