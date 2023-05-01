#include "Camera.h"

Camera::Camera(bool useEye1) : cameraplane{ CAMERA_PLANE_HEIGHT }
{
	//[0,0] defined as the lower left corner of the camera plane [height - 1, width - 1] -> top right corner
	for (size_t row{ 0 }; row < CAMERA_PLANE_HEIGHT; ++row) {
		for (size_t column{ 0 }; column < CAMERA_PLANE_WIDTH; ++column) {
			double y{ row * DELTA_HEIGHT - (1.0 - DELTA_HEIGHT / 2.0) };
			double z{ column * DELTA_HEIGHT - (1.0 - DELTA_WIDTH / 2.0) };

			glm::dvec3 rayStartPoint{ useEye1 ? eyePos1 : eyePos2 };
			glm::dvec3 rayEndpoint{ 0.0, y, z };
			glm::dvec3 rayDir{ rayEndpoint - rayStartPoint };
			glm::dvec3 importance{ glm::dvec3{1.0, 1.0, 1.0} };

			std::vector<Ray> rays;

			Ray ray{ rayStartPoint, rayDir, importance };
			rays.push_back(ray);

			cameraplane[row].push_back(Pixel{ rays });
		}
	}
}

void Camera::render(Scene const& scene)
{
	for (size_t row{ 0 }; row < CAMERA_PLANE_HEIGHT; ++row) {
		for (size_t column{ 0 }; column < CAMERA_PLANE_WIDTH; ++column) {

			Pixel& pixel{ cameraplane[row][column] };
			for (Ray& ray : pixel.rays) {
				//Shoot the ray and build up a raytree
				//scene.shootRayIntoScene(ray);
				int maxDepth{ 3 };
				RayTree rayTree{ scene, ray, maxDepth };
				rayTree.createRayTree();

				//Traverse raytree in backwards order to compute the color 
				pixel.color += rayTree.computeRadiance();
			}
		}
	}
}


void Camera::writeToFile(std::string const& filename) {
	EasyBMP::Image image(CAMERA_PLANE_WIDTH, CAMERA_PLANE_HEIGHT, filename, EasyBMP::RGBColor(255, 105, 180));
	
	constexpr double minD{ std::numeric_limits<double>::min() };
	//double maxR{ minD }, maxB{ minD }, maxG{ minD };
	double maxValue{ minD };

	std::vector<std::vector<glm::dvec3>> scaledColor(CAMERA_PLANE_HEIGHT);
	for (size_t row{ 0 }; row < CAMERA_PLANE_HEIGHT; ++row) {
		for (size_t column{ 0 }; column < CAMERA_PLANE_WIDTH; ++column) {
			glm::dvec3 color{ cameraplane[row][column].color };

			color.r = std::sqrt(color.r);
			color.g = std::sqrt(color.g);
			color.b = std::sqrt(color.b);

			cameraplane[row][column].color = color;

			//maxR = std::max(color.r, maxR);
			//maxG = std::max(color.g, maxG);
			//maxB = std::max(color.b, maxB);
			
			double max = std::max(std::max(color.r, color.g), color.b);
			maxValue = std::max(max, maxValue);
			
		}
	}

	//Write to file, important to fetch in reverse row order to write bottom to up  
	//as we have defined the camera plane, otherwise image would be upside-down.
	for (size_t row{ 0 }; row < CAMERA_PLANE_HEIGHT; ++row) {
		for (size_t column{ 0 }; column < CAMERA_PLANE_WIDTH; ++column) {
			glm::dvec3 color = cameraplane[row][column].color;

			//color.r /= maxR;
			//color.g /= maxG;
			//color.b /= maxB;
			color.r /= maxValue;
			color.g /= maxValue;
			color.b /= maxValue;

			image.SetPixel((CAMERA_PLANE_HEIGHT - 1) - row, (CAMERA_PLANE_WIDTH - 1) - column, EasyBMP::RGBColor(
				color.r * 255,
				color.g * 255,
				color.b * 255),
				false
			);
		}
	}

	//Render scene in gray scale gradient
	//for (size_t row{ 0 }; row < CAMERA_PLANE_HEIGHT; ++row) {
	//	for (size_t column{ 0 }; column < CAMERA_PLANE_WIDTH; ++column) {
	//		glm::dvec3 color = cameraplane[row][column].color;

	//		image.SetPixel(row, (CAMERA_PLANE_HEIGHT - 1) - column, EasyBMP::RGBColor(
	//			row / static_cast<float>(CAMERA_PLANE_HEIGHT) * 255 * column / static_cast<float>(CAMERA_PLANE_WIDTH),
	//			row / static_cast<float>(CAMERA_PLANE_HEIGHT) * 255 * column / static_cast<float>(CAMERA_PLANE_WIDTH),
	//			row / static_cast<float>(CAMERA_PLANE_HEIGHT) * 255 * column / static_cast<float>(CAMERA_PLANE_WIDTH)),
	//			false
	//		);
	//	}
	//}
	image.Write();
}