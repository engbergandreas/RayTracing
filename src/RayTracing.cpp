#pragma once

#include <string>

#include "Camera.h"
#include "Triangle.h"
#include "Scene.h"
#include "Tetrahedron.h"

int main()
{
	Scene scene{};
	std::string filename{ "../images/test.bmp" };
	Camera camera{ false };

	Triangle t{ {0.0, 6.0, -5.0}, {10.0, 6.0, 6.0}, {0.0, 6.0, 5.0}, {0.0, 0.0, 0.0} };
	Tetrahedron tetra{ glm::dvec3{1.0, 0.2, 0.3} };


	camera.render(scene);
	camera.writeToFile(filename);
	return 0;
}
