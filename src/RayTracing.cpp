#pragma once

#include <string>

#include "Camera.h"
#include "Triangle.h"
#include "Scene.h"
#include "Tetrahedron.h"
#include <chrono>

int main()
{
	Scene scene{};
	std::string filename{ "../images/test.bmp" };
	
	int const supersampling{ 2 };
	Camera camera{ false, supersampling };
	int maxDepth{ 3 };

	auto start = std::chrono::high_resolution_clock::now();
	camera.render(scene, maxDepth);
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
	std::cout << "Finished rendering in: " << duration.count() << " seconds\n";

	camera.writeToFile(filename);
	return 0;
}
