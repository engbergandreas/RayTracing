#pragma once

#include <string>

#include "Camera.h"
#include "Triangle.h"
#include "Scene.h"
#include "Tetrahedron.h"
#include <chrono>
#include "Utils.h"
#include <thread>

int main()
{
	//std::cout << nThreads << " concurrent threads are supported\n";
	Scene scene{};
	std::string filename{ settings::FILENAME };
	
	Camera camera{ false, settings::SUPERSAMPLING };

	auto start = std::chrono::high_resolution_clock::now();
	camera.render(scene);
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
	std::cout << "Finished rendering in: " << duration.count() << " seconds\n";

	camera.writeToFile(filename);
	return 0;
}
