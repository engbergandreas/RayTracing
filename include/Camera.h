#ifndef CAMERA_H_
#define CAMERA_H_

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Ray.h"
#include "EasyBMP.h"
#include "Scene.h"
#include "Raytree.h"
#include <random>

//Forward decl
class Scene;

int const CAMERA_PLANE_WIDTH{ 800 };
int const CAMERA_PLANE_HEIGHT{ 800 };
double const DELTA_WIDTH{ 2.0 / CAMERA_PLANE_WIDTH };
double const DELTA_HEIGHT{ 2.0 / CAMERA_PLANE_HEIGHT };

class Camera {
public:
	Camera(bool useeye1 = true, int ss = 0);
	void render(Scene const& scene);
	void writeToFile(std::string const& filename);

private:
	struct Pixel {
		Pixel(std::vector<Ray> const& _rays) : rays{ _rays } {}
		glm::dvec3 color{ 0.0, 0.0, 0.0 };
		std::vector<Ray> rays;
	}; //Pixel 

	//Camera plane defined as Row<Column>
	std::vector<std::vector<Pixel>> cameraplane;
	glm::dvec3 eyePos1{ -2.0, 0.0, 0.0 };
	glm::dvec3 eyePos2{ -1.0, 0.0, 0.0 };

	int const _supersampling;
};
#endif // !CAMERA_H_