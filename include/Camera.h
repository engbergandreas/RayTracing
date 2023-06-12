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
#include <chrono>
#include "Utils.h"
#include <thread>
#include <mutex>

//Forward decl
class Scene;

int const CAMERA_PLANE_WIDTH{ settings::CAMERA_PLANE_WIDTH };
int const CAMERA_PLANE_HEIGHT{ settings::CAMERA_PLANE_HEIGHT };
double const DELTA_WIDTH{ settings::DELTA_WIDTH };
double const DELTA_HEIGHT{ settings::DELTA_HEIGHT };

class Camera {
public:
	Camera(glm::dvec3 const& pos, glm::dvec3 const& front, glm::dvec3 const& up);
	void render(Scene const& scene);
	void writeToFile(std::string const& filename);

	glm::dmat4 cameraToWorld() const;
private:
	struct Pixel {
		Pixel(/*std::vector<Ray> const& _rays*/) /*: rays{ _rays }*/ {}
		glm::dvec3 color{ 0.0, 0.0, 0.0 };
		//std::vector<Ray> rays;
	}; //Pixel 

	std::vector<Ray> createPixelRays(size_t row, size_t column);
	void computePixelColor(size_t rowstart, size_t rowend, Scene const& scene, std::chrono::steady_clock::time_point const& begin);
	void updateTimeEstimate(std::chrono::steady_clock::time_point const& begin);

	//Camera plane defined as Row<Column>
	std::vector<std::vector<Pixel>> cameraplane;

	int const _supersampling;
	std::mutex _renderPixelLock;
	int _renderedPixels{ 0 };
	int _pixelSinceLastUpdate{ 0 };
	double _totalAverageTimePerPixel{ 0.0 };
	glm::dmat4 _viewMatrix;

	std::chrono::steady_clock::time_point _lastTimeCheck;
};
#endif // !CAMERA_H_