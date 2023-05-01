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

	camera.render(scene);
	camera.writeToFile(filename);
	return 0;
}
