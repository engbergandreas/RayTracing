#include "Camera.h"

Camera::Camera(bool useEye1, int ss) : cameraplane{ CAMERA_PLANE_HEIGHT }, _supersampling{ ss }
{
	std::random_device rd; //Used as seed for generator, random device is a non-deterministic number generator
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()

	//[0,0] defined as the lower left corner of the camera plane [height - 1, width - 1] -> top right corner
	for (size_t row{ 0 }; row < CAMERA_PLANE_HEIGHT; ++row) {
		for (size_t column{ 0 }; column < CAMERA_PLANE_WIDTH; ++column) {

			double const y{ row * DELTA_HEIGHT - (1.0 - DELTA_HEIGHT / 2.0) };
			double const z{ column * DELTA_HEIGHT - (1.0 - DELTA_WIDTH / 2.0) };
			
			std::vector<Ray> rays;
			glm::dvec3 importance{ glm::dvec3{1.0, 1.0, 1.0} };

			//This is a fallback if we don't want super sampling to get atleast one ray per pixel.
			if (_supersampling < 2) {
				glm::dvec3 rayStartPoint{ useEye1 ? eyePos1 : eyePos2 };
				glm::dvec3 rayEndpoint{ 0.0, y, z };
				glm::dvec3 rayDir{ rayEndpoint - rayStartPoint };
				Ray ray{ rayStartPoint, rayDir, importance };
				rays.push_back(ray);
			}

			//Create subpixels in a uniform grid 
			for(int r{-_supersampling/2}; r <= _supersampling/2; ++r){
				if (r == 0)
					continue;
				for(int c{-_supersampling /2}; c <= _supersampling / 2; ++c){
					if (c == 0)
						continue;
					//Center of subpixel
					double y_ss = y + r * DELTA_HEIGHT / (2.0 * _supersampling);
					double z_ss = z + c * DELTA_WIDTH / (2.0 * _supersampling);
					//Get min and max of subpixel
					double ymin = y_ss - DELTA_HEIGHT / (2.0 + _supersampling);
					double ymax = y_ss + DELTA_HEIGHT / (2.0 + _supersampling);
					double zmin = z_ss - DELTA_WIDTH / (2.0 + _supersampling);
					double zmax = z_ss + DELTA_WIDTH / (2.0 + _supersampling);
					//Create distribution to draw random number from in the range [min, max)
					std::uniform_real_distribution<> disY{ ymin, ymax };
					std::uniform_real_distribution<> disZ{ zmin, zmax };
					//Get random point in subpixel
					double rY = disY(gen);
					double rZ = disZ(gen);

					glm::dvec3 rayStartPoint{ useEye1 ? eyePos1 : eyePos2 };
					glm::dvec3 rayEndpoint{ 0.0, rY, rZ };
					glm::dvec3 rayDir{ rayEndpoint - rayStartPoint };
					Ray ray{ rayStartPoint, rayDir, importance };
					rays.push_back(ray);
				}
			}

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

			//This is not necessary as it will scale all pixels by the same amount. 
			//However if there would be some form of dynamic rays per pixel then we would need to scale 
			//the colors to make the scene correct. 
			//Divide color contribution by number of rays per pixel
			//if (_supersampling > 1) {
				//pixel.color /= static_cast<double>(_supersampling * _supersampling);
			//}
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