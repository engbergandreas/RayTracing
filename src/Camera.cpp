#include "Camera.h"

Camera::Camera(glm::dvec3 const& pos, glm::dvec3 const& front, glm::dvec3 const& up) : 
	cameraplane{ CAMERA_PLANE_HEIGHT, std::vector<Pixel>{ CAMERA_PLANE_WIDTH } }, _supersampling{ settings::SUPERSAMPLING } {
	//Look at creates a world to camera matrix, so we inverse it to get a camera to world
	_viewMatrix = glm::inverse(glm::lookAt(pos, pos + front, up)); 
}

std::vector<Ray> Camera::createPixelRays(size_t row, size_t column) { //This should rather be const, but it generates error C3848 for _gen
	//Src: https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays.html
	//This is the idea used in the for loop below, computing y and z
	/*
	//Raster space [0,PLANE_WIDTH/HEIGHT)
	double pixelY = row + 0.5; //Get the center of the pixel
	double pixelZ = row + 0.5;
	//NDC space (0,1)
	double ndcY = pixelY / CAMERA_PLANE_WIDTH;
	double ndcZ = pixelZ / CAMERA_PLANE_HEIGHT;
	//screen space [-1,1]
	double pixelScreenY = 2.0 * ndcY - 1.0; //We change here to get positive values above the z axis and negative below. 
	double pixelScreenZ = 1.0 - 2.0 * ndcZ;
	//Multiply with aspect ratio to get square pixels.
	double aspect{ static_cast<double>(CAMERA_PLANE_WIDTH) / static_cast<double>(CAMERA_PLANE_HEIGHT) };
	pixelScreenY *= aspect;
	*/

	std::vector<Ray> rays{};
	glm::dvec3 importance{ glm::dvec3{1.0, 1.0, 1.0} };

	//Generate SxS subpixels with random y,z within this pixel. 
	for (int i{ 0 }; i < _supersampling * _supersampling; i++) {
		double ry{ utils::threadSafeRandom(0.0, 1.0) };
		double rz{ utils::threadSafeRandom(0.0, 1.0) };
		double y = 2.0 * (column + ry) / CAMERA_PLANE_WIDTH - 1.0;
		y *= settings::ASPECT_RATIO;
		double z = 1.0 - 2.0 * (row + rz) / CAMERA_PLANE_HEIGHT;

		glm::dvec3 rayStartPoint{ 0.0, 0.0, 0.0 }; //Defined in local coords
		glm::dvec3 rayEndpoint{ 1.0, z, y }; //Defined in local coords
		glm::dvec3 rayStartWorld = _viewMatrix * glm::dvec4{rayStartPoint, 1.0};
		glm::dvec3 rayEndWorld = _viewMatrix * glm::dvec4{rayEndpoint, 1.0};
		glm::dvec3 rayDir{ rayEndWorld - rayStartWorld };
		Ray ray{ rayStartWorld, rayDir, importance };
		rays.push_back(ray);
	}
	return rays;

	//This solution creates a uniform grid where we pick a random location within each subpixel
	/*
	* OBS: random number generator is not thread safe.
	double const z{ 1.0 - row * DELTA_HEIGHT - (DELTA_HEIGHT / 2.0) };
	double const y{ column * DELTA_WIDTH - (1.0 - DELTA_WIDTH / 2.0) };

	//std::vector<Ray> rays;
	//glm::dvec3 importance{ glm::dvec3{1.0, 1.0, 1.0} };

	//This is a fallback if we don't want super sampling to get atleast one ray per pixel.
	if (_supersampling < 2) {
		glm::dvec3 rayStartPoint{ _useEye1 ? eyePos1 : eyePos2 };
		glm::dvec3 rayEndpoint{ 0.0, y, z };
		glm::dvec3 rayDir{ rayEndpoint - rayStartPoint };
		Ray ray{ rayStartPoint, rayDir, importance };
		rays.push_back(ray);
	}
	else {
		//Create subpixels in a uniform grid 
		for (int r{ -_supersampling / 2 }; r <= _supersampling / 2; ++r) {
			if (r == 0)
				continue;
			for (int c{ -_supersampling / 2 }; c <= _supersampling / 2; ++c) {
				if (c == 0)
					continue;
				//Center of subpixel
				double y_ss = y + r * DELTA_WIDTH / (2.0 * _supersampling);
				double z_ss = z + c * DELTA_HEIGHT / (2.0 * _supersampling);
				//Get min and max of subpixel
				double ymin = y_ss - DELTA_WIDTH / (2.0 + _supersampling);
				double ymax = y_ss + DELTA_WIDTH / (2.0 + _supersampling);
				double zmin = z_ss - DELTA_HEIGHT / (2.0 + _supersampling);
				double zmax = z_ss + DELTA_HEIGHT / (2.0 + _supersampling);
				//Create distribution to draw random number from in the range [min, max)
				std::uniform_real_distribution<> disY{ ymin, ymax };
				std::uniform_real_distribution<> disZ{ zmin, zmax };
				//Get random point in subpixel
				double rY = disY(_gen);
				double rZ = disZ(_gen);

				glm::dvec3 rayStartPoint{ -1.0, 0.0, 0.0 };
				glm::dvec3 rayEndpoint{ 0.0, rY, rZ };
				glm::dvec3 rayDir{ rayEndpoint - rayStartPoint };
				Ray ray{ rayStartPoint, rayDir, importance };
				rays.push_back(ray);
			}
		}
	}
	return rays;
	*/
}

void Camera::updateTimeEstimate(std::chrono::steady_clock::time_point const& begin) {
	int const totalPixels{ CAMERA_PLANE_HEIGHT * CAMERA_PLANE_WIDTH };

	std::chrono::steady_clock::time_point pixelBatchEnd = std::chrono::steady_clock::now();
	long long int secondsSinceLastCheck{ std::chrono::duration_cast<std::chrono::seconds>(pixelBatchEnd - _lastTimeCheck).count() };
	double avgTimePerPixel = static_cast<double>(secondsSinceLastCheck) / static_cast<double>(_renderedPixels - _pixelSinceLastUpdate);
	_totalAverageTimePerPixel += avgTimePerPixel;
	double pixelBatchCount = _renderedPixels / static_cast<double>(totalPixels) * 10;

	std::cout << "Rendered " << _renderedPixels / static_cast<double>(totalPixels) * 100 << "% of pixels in " <<
		std::chrono::duration_cast<std::chrono::seconds>(pixelBatchEnd - begin).count() << " seconds\n" <<
		//"Estimated time remaining: " << avgTimePerPixel * (totalPixels - renderedPixels) << " seconds\n";
		"Estimated time remaining: " << (_totalAverageTimePerPixel / pixelBatchCount) * (totalPixels - _renderedPixels) << " seconds\n";

	_lastTimeCheck = pixelBatchEnd;
	_pixelSinceLastUpdate = _renderedPixels;
}

void Camera::computePixelColor(size_t rowstart, size_t rowend, Scene const& scene, std::chrono::steady_clock::time_point const& begin) {
	int const totalPixels{ CAMERA_PLANE_HEIGHT * CAMERA_PLANE_WIDTH };
	//Obs, boundary check must be done!
	//[0,0] defined as the lower left corner of the camera plane [height - 1, width - 1] -> top right corner
	for (size_t row{ rowstart }; row < rowend && row < CAMERA_PLANE_HEIGHT; ++row) {
		for (size_t column{ 0 }; column < CAMERA_PLANE_WIDTH; ++column) {

			std::vector<Ray> rays{ createPixelRays(row, column) };
			Pixel& pixel{ cameraplane[row][column] };
			for (Ray& ray : rays) {
				//Shoot the ray and build up a raytree
				RayTree rayTree{ scene, ray };
				rayTree.createRayTree();

				//Traverse raytree in backwards order to compute the color 
				pixel.color += rayTree.computeRadiance();

				//This is not necessary as it will scale all pixels by the same amount. 
				//However if there would be some form of dynamic rays per pixel then we would need to scale 
				//the colors to make the scene correct. 
				//Divide color contribution by number of rays per pixel
				//if (_supersampling > 1) {
					//pixel.color /= static_cast<double>(_supersampling * _supersampling);
				//}
			}
			//Critical section
			{
				//Lock mutex
				std::unique_lock<std::mutex> lock{ _renderPixelLock };
				++_renderedPixels;
				//Estimate remaining time, output every 10% of pixels rendered
				if (_renderedPixels % (totalPixels / 10) == 0)
					updateTimeEstimate(begin);
			} //Mutex is released on exiting critical section
		}
	}
}

void Camera::render(Scene const& scene)
{
	//Estimate remaining time to render every 10% of pixels
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	_lastTimeCheck = begin;

	if (settings::USE_MULTI_THREADING) {
		//Use multithreading where each thread will compute
		//some % of the pixels. TODO: find a better split between threads and pixels, as the workload
		//can become uneven depending on where computationally heavy objects in the scene are located.
		unsigned int const nThreads{ std::thread::hardware_concurrency() };
		std::vector<std::thread> workers{};
		for (unsigned int i{ 0 }; i < nThreads; i++) {
			//Take the ceil of height / nthreads if the division gives a rest product this would miss a row in the output. 
			size_t nRowsPerThread{ static_cast<size_t>(std::ceil(static_cast<double>(CAMERA_PLANE_HEIGHT) / nThreads)) };
			size_t rowStart{ nRowsPerThread * i };
			size_t rowEnd{ rowStart + nRowsPerThread };

			std::thread worker(&Camera::computePixelColor, this, rowStart, rowEnd, std::ref(scene), std::ref(begin));
			workers.push_back(std::move(worker));
		}

		//Wait for all workers (threads) to finish their work.
		for (std::thread& worker : workers) {
			worker.join();
		}
	}
	else {
		//Use single thread
		size_t rowStart{ 0 };
		computePixelColor(rowStart, CAMERA_PLANE_HEIGHT, scene, begin);
	}
}

glm::dmat4 Camera::cameraToWorld() const {
	return _viewMatrix;
}

void Camera::writeToFile(std::string const& filename) {
	EasyBMP::Image image(CAMERA_PLANE_WIDTH, CAMERA_PLANE_HEIGHT, filename, EasyBMP::RGBColor(255, 105, 180));
	
	constexpr double minD{ std::numeric_limits<double>::min() };
	//double maxR{ minD }, maxB{ minD }, maxG{ minD };
	double maxValue{ minD };

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

			image.SetPixel(column, row, EasyBMP::RGBColor(
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

	//		image.SetPixel(/*(CAMERA_PLANE_WIDTH - 1) - */column, /*CAMERA_PLANE_HEIGHT - 1 -*/ row, EasyBMP::RGBColor(
	//			row / static_cast<float>(CAMERA_PLANE_HEIGHT) * 255  /*column / static_cast<float>(CAMERA_PLANE_WIDTH)*/,
	//			row / static_cast<float>(CAMERA_PLANE_HEIGHT) * 255  /*column / static_cast<float>(CAMERA_PLANE_WIDTH)*/,
	//			row / static_cast<float>(CAMERA_PLANE_HEIGHT) * 255  /*column / static_cast<float>(CAMERA_PLANE_WIDTH)*/),
	//			false
	//		);
	//	}
	//}
	image.Write();
}