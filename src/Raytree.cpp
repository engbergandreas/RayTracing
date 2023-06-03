#include "Raytree.h"

RayTree::RayTree(Scene const& scene, Ray& ray) :
	_root{ new Node{ ray } }, _scene{ scene } {
	std::random_device rd; //Used as seed for generator, random device is a non-deterministic number generator

	_generator = std::mt19937(rd()); //Standard mersenne_twister_engine seeded with rd()
	//Create uniform distribution between (0,1)
	_theta_distribution = std::uniform_real_distribution<>{ 0.0, 1.0 };
	_rho_distribution = std::uniform_real_distribution<>{ 0.0, 1.0 };
}

void RayTree::createRayTree()
{
	buildRayTree(_root, 0);
}

glm::dvec3 RayTree::computeRadiance()
{
	return computeRayColor(_root);
}

void RayTree::buildRayTree(Node* ptr, int depth)
{
	//Shoot the ray into the scene, check that it hit something.
	//If it hit something create a reflected and refracted ray,
	//continue recursively with the reflected and refracted rays.
	//Rays will compute and store importance carried from camera.

	_scene.shootRayIntoScene(ptr->_ray);

	//Determine if we should terminate this ray path.
	if (terminateRayPath(ptr, depth))
		return;

	++depth;

	//Ray intersect diffuse surface, cast reflection ray in random direction.
	if (ptr->_ray.hitinfo->brdf().isDiffuse())
	{
		Node* diffuseReflection{ computeDiffiuseReflection(ptr->_ray) };
		if (diffuseReflection) {
			diffuseReflection->_parent = ptr;
			ptr->_reflected = diffuseReflection;
			buildRayTree(ptr->_reflected, depth);
		}

		//This might actually cause an overestimate in the color value
		//since computeRadiance will add both child radiance at the intersection.
		//Should probably divide the radiance contribution by 2 if we use multiple reflections?
		if (settings::MULTIPLE_DIFFUSE_REFLECTIONS) {
			//Send another diffuse reflection ray
			Node* diffuseReflection2{ computeDiffiuseReflection(ptr->_ray) };
			if (diffuseReflection2) {
				diffuseReflection2->_parent = ptr;
				ptr->_refracted = diffuseReflection2;
				buildRayTree(ptr->_refracted, depth);
			}
		}
		return;
	}

	//Compute reflection ray direction
	Node* reflectedRay{ computeReflectedRay(ptr->_ray) };
	reflectedRay->_parent = ptr;
	ptr->_reflected = reflectedRay;

	//Continue to build ray tree
	buildRayTree(ptr->_reflected, depth);

	//If we did not hit a transparent object, dont shoot refraction rays.
	if (ptr->_ray.hitinfo->brdf()._material != Material::transparent)
		return;

	//Compute refraction direction
	Node* refractedRay{ computeRefractedRay(ptr->_ray) };

	if (!refractedRay)
		return;

	refractedRay->_parent = ptr;
	ptr->_refracted = refractedRay;

	//Continue to build ray tree
	buildRayTree(ptr->_refracted, depth);
}

glm::dvec3 RayTree::computeRayColor(Node* ptr)
{
	//Check if first ray launched from camera directly hit the lightsource.
	if (!ptr->_parent && ptr->_ray.hitinfo) {
		// If lightsource was hit by the ray we take the radiance L0 directly from the lightsource
		if (ptr->_ray.hitinfo->brdf()._material == Material::lightsource) {
			//Divide the contribution by the number of rays casted per pixel
			//Imo gives better looking results by reducing the scalar division, altough not entirely correct.
			//Lecture 10, slide 6
			int const raysPerPixel{ settings::SUPERSAMPLING /** settings::SUPERSAMPLING */};
			//Lightsource store the radiance in the color channels
			return ptr->_ray.hitinfo->color() / static_cast<double>(raysPerPixel);
		}
	}

	glm::dvec3 reflectedRadiance{ 0.0, 0.0,0.0 };
	glm::dvec3 refractedRadiance{ 0.0, 0.0,0.0 };
	glm::dvec3 reflectedImportance{ 0.0, 0.0, 0.0 };
	glm::dvec3 refractedImportance{ 0.0, 0.0, 0.0 };

	//We are at leaf node of tree, compute radiance using shadow rays.
	if (!ptr->_reflected && !ptr->_refracted)
	{
		//In an ideal world all rays would hit an object
		//this is a backup if a ray for some reason did not hit an object e.g., due to numerical errors
		if (ptr->_ray.hitinfo) {
			//Cast shadow rays which introduce radiance to the tree. Only at diffuse surfaces!
			if (ptr->_ray.hitinfo->brdf().isDiffuse())
				return shootShadowRay(ptr);

			//If we hit the lightsource directly after hitting a mirror we will contribute a larger 
			//portion of the lightsrc radiance, this is to create more realistic reflections of lightsources
			//in the mirror.
			if (ptr->_ray.hitinfo->brdf()._material == Material::lightsource && 
				ptr->_parent->_ray.hitinfo->brdf()._material == Material::specular) {
				int const raysPerPixel{ settings::SUPERSAMPLING /** settings::SUPERSAMPLING */ };
				//Lightsource store the radiance in the color channels
				return ptr->_ray.hitinfo->color() / static_cast<double>(raysPerPixel);
			}

		}
		return glm::dvec3{ 0.0,0.0,0.0 };
	}

	//If we are not at a leaf node we continue down the tree
	if (ptr->_reflected) {
		reflectedRadiance = computeRayColor(ptr->_reflected);
		reflectedImportance = ptr->_reflected->_ray.importance;
	}

	if (ptr->_refracted) {
		reflectedRadiance = computeRayColor(ptr->_refracted);
		refractedImportance = ptr->_refracted->_ray.importance;
	}

	//This will and should never happen, however if this check is not made we get a warning when computing localRadiance.
	if (!ptr->_ray.hitinfo) {
		//std::cout << "This should never happen \n";
		return glm::dvec3{ 0.0 };
	}

	//Compute local contribution at every diffuse surface intersection by shooting shadow rays.
	//Don't shoot shadow rays if we are at a transparent or specular or any other type of object.
	glm::dvec3 localRadiance{ ptr->_ray.hitinfo->brdf().isDiffuse() ? shootShadowRay(ptr) : glm::dvec3{0.0, 0.0, 0.0} };
	double LD_coeff{ settings::LOCAL_DIFFUSE_LIGHT_COEFFICIENT };

	//Radiance of parent is the combined radiance from reflected and refracted rays 
	//plus that of the local illumination model.
	//Due to color bleeding between diffuse surfaces, the importance can become 0 in one of the channels
	//This avoid division by zero if that is the case. 
	glm::dvec3 radiance{ 0.0 };
	if (ptr->_ray.importance.r > 0.0)
		radiance.r = (reflectedRadiance.r * reflectedImportance.r + refractedRadiance.r * refractedImportance.r) / ptr->_ray.importance.r;
	if (ptr->_ray.importance.g > 0.0)
		radiance.g = (reflectedRadiance.g * reflectedImportance.g + refractedRadiance.g * refractedImportance.g) / ptr->_ray.importance.g;
	if (ptr->_ray.importance.b > 0.0)
		radiance.b = (reflectedRadiance.b * reflectedImportance.b + refractedRadiance.b * refractedImportance.b) / ptr->_ray.importance.b;

	//Add local lightning contribution.
	radiance += localRadiance * LD_coeff;

	return radiance;
}

bool RayTree::terminateRayPath(Node* ptr, int depth) const {
	//Terminate on:

	//Ray did not hit anything, ideally it should not happen (closed room).
	if (!ptr->_ray.hitinfo)
		return true;

	//Exit early if ray contribution will not improve image quality
	if (ptr->_ray.importance.r < settings::IMPORTANCE_THRESHOLD &&
		ptr->_ray.importance.g < settings::IMPORTANCE_THRESHOLD &&
		ptr->_ray.importance.b < settings::IMPORTANCE_THRESHOLD)
		return true;

	//Exit early if we have reached transparent max depth,
	//can be lower than max ray depth
	if ((depth > settings::MAX_TRANSPARENT_DEPTH) &&
		ptr->_ray.hitinfo->brdf()._material == Material::transparent)
		return true;

	//If we hit a lightsource we stop tracing. 
	if (ptr->_ray.hitinfo->brdf()._material == Material::lightsource)
		return true;

	//We have reached an upper limit of rays.
	if (depth > settings::MAX_RAY_DEPTH)
		return true;

	return false;
}

glm::dvec3 RayTree::shootShadowRay(Node* ptr) {
	//Shoot shadow rays at area lightsource, compute their radiance contribution. 
	glm::dvec3 radiance{ 0.0 };
	for (Lightsource const& lightsource : _scene.getLightSources()) {
		double sideLength{ lightsource.sideLength() };
		glm::dvec3 const& startPosition{ ptr->_ray.intersectionPoint };
		glm::dvec3 intersectionNormal{ ptr->_ray.hitinfo->getNormal(startPosition) };

		glm::dvec3 irradiance{ 0.0 };
		size_t const NUMBER_SHADOW_RAYS{ settings::NUMBER_OF_SHADOW_RAYS };
		for (size_t i{ 0 }; i < NUMBER_SHADOW_RAYS; i++) {
			double c1{ _rho_distribution(_generator) * sideLength };
			double c2{ _rho_distribution(_generator) * sideLength };
			glm::dvec3 pointOnLightsrc{ std::move(lightsource.getRandomPointOnSurface(c1,c2)) };

			glm::dvec3 shadowDirection{ pointOnLightsrc - startPosition };
			Ray shadowRay{ startPosition, shadowDirection, glm::dvec3{0.0} };

			//Point is in shadow by other object, no light contribution. 
			if (!isShadowRayVisible(shadowRay)) {
				continue;
			}

			//Geometric term
			glm::dvec3 Sdir{ shadowRay.rayDirection() };
			double cosThetaIn{ glm::dot(Sdir, intersectionNormal) };
			double cosThetaL{ glm::dot(-Sdir, lightsource.normal(pointOnLightsrc)) };
			cosThetaIn = glm::clamp(cosThetaIn, 0.0, 1.0);
			cosThetaL = glm::clamp(cosThetaL, 0.0, 1.0);

			double GeometricTerm{ cosThetaIn * cosThetaL / glm::dot(shadowDirection, shadowDirection) };
			
			irradiance += GeometricTerm;
		}
		//Lecture 10 slide 11
		glm::dvec3 sigma{ ptr->_ray.hitinfo->brdf().lambertianReflectionCoeff };
		radiance += sigma * ptr->_ray.hitinfo->color() * lightsource.L0() * sideLength * sideLength * irradiance /
			(static_cast<double>(NUMBER_SHADOW_RAYS) * M_PI);
	}
	return radiance / static_cast<double>(_scene.getLightSources().size());


	/*Code used for point lightsource*/
	//Lightsource light{ _scene.getLightSources().back()};
	//glm::dvec3 intersectionPoint{ ptr->_ray.intersectionPoint };
	//glm::dvec3 sRayEndPoint{ light._position };
	//glm::dvec3 sRayDir{ sRayEndPoint - intersectionPoint };

	//Ray shadowRay{ intersectionPoint, sRayDir, glm::dvec3{ 0.0 } };

	////Check if shadow ray is intersecting another object before reaching light source
	//std::vector<Intersectable*> const& objects{ _scene.getObjects() };
	//for (Intersectable const*  obj : objects) {
	//	//Simplification, shadowrays ignore transparent objects.
	//	if (obj->brdf()._material == Material::transparent ||
	//		obj->brdf()._material == Material::lightsource)
	//		continue;

	//	double t = obj->rayIntersection(shadowRay);
	//	if (t > 0.0 && t < (1.0 - EPS)) {
	//		return glm::dvec3{ 0.0, 0.0, 0.0 }; //Point is in shadow of another object, no light contribution.
	//	}
	//}

	//double r{ glm::length(shadowRay.rayDirection(false)) };

	//double cosTheta{ glm::dot(shadowRay.rayDirection(), ptr->_ray.hitinfo->getNormal(intersectionPoint)) };
	//cosTheta = glm::clamp(cosTheta, 0.0, 1.0);
	//
	//double Sigma{ light.cross_section / (r * r) };
	//glm::dvec3 irradiance{ Sigma * cosTheta * light._L0 };

	//glm::dvec3 rho{ ptr->_ray.hitinfo->brdf().lambertianReflectionCoeff };
	//glm::dvec3 emittedLight{ rho * irradiance / M_PI };

	//return ptr->_ray.hitinfo->color() * emittedLight;
}

bool RayTree::isShadowRayVisible(Ray const& shadowRay) const {
	for (Intersectable const* obj : _scene.getObjects()) {
		if (obj->brdf()._material == Material::transparent ||
			obj->brdf()._material == Material::lightsource) //TODO: check if the lightsource we are aiming for is the obj, other lightsources might still block
			continue;

		double t = obj->rayIntersection(shadowRay);
		if (t > 0.0 && t < (1.0 - EPS)) {
			return false; //Point is in shadow of another object.
		}
	}

	return true;
}

RayTree::Node* RayTree::computeReflectedRay(Ray const& inRay)
{
	glm::dvec3 surfaceNormal{ inRay.hitinfo->getNormal(inRay.intersectionPoint) };
	glm::dvec3 reflectedDir{ glm::reflect(inRay.rayDirection(), surfaceNormal) };
	//Add an offset in the direction of the surface normal 
	glm::dvec3 offset = surfaceNormal * EPS;
	//If we are inside the object we want to move in the negative normal direction
	if (glm::dot(inRay.rayDirection(), surfaceNormal) > 0.0)
		offset = -offset;

	//We use perfect reflection, but if we hit transparent object we compute 
	//reflective coefficient using Schlick's approximation.
	glm::dvec3 reflectiveCoefficient{ inRay.hitinfo->brdf().lambertianReflectionCoeff };
	//Lecture 5 page 10
	if (inRay.hitinfo->brdf()._material == Material::transparent) {
		double cosTheta{ glm::dot(surfaceNormal, -inRay.rayDirection()) };
		double n1{ constants::IOR_AIR }; //ior air
		double n2{ inRay.hitinfo->brdf().ior };
		double R0{ ((n1 - n2) / (n1 + n2)) * ((n1 - n2) / (n1 + n2)) };
		reflectiveCoefficient = glm::dvec3{ R0 + (1 - R0) * std::pow(1 - cosTheta, 5) };
	}

	glm::dvec3 reflectedImportance{ inRay.importance  * reflectiveCoefficient };//inRay.hitinfo->brdf().lambertianReflectionCoeff };
	Ray reflectedRay{ inRay.intersectionPoint + offset, reflectedDir, reflectedImportance };

	return new Node{ reflectedRay };
}

RayTree::Node* RayTree::computeRefractedRay(Ray const& inRay)
{
	double n1{ 1.0 }; //ior air
	double n2{ inRay.hitinfo->brdf().ior };

	glm::dvec3 surfaceNormal{ inRay.hitinfo->getNormal(inRay.intersectionPoint) };
	double cosTheta{ glm::dot(surfaceNormal, -inRay.rayDirection()) };
	glm::dvec3 refractedDir{ 0.0 };

	bool entering{ cosTheta > 0.0 };
	//We go from lower medium to higher, there will always be a refracted ray
	if (entering) {
		refractedDir = glm::refract(inRay.rayDirection(), surfaceNormal, n1 / n2);
	}
	else {
		//We go from higher medium to lower, we must take Brewster angle into consideration
		//Need to reverse normal direction as it was computed for the outside, same with cosTheta.
		surfaceNormal = -surfaceNormal;
		//double cosTheta{ glm::dot(surfaceNormal, -inRay.rayDirection()) };
		cosTheta = -cosTheta;

		//Computations are based on: http://www.cs.uu.nl/docs/vakken/magr/2019-2020/slides/lecture%2002%20-%20whitted.pdf
		//and https://www.flipcode.com/archives/reflection_transmission.pdf
		//As well as lecture 5.

		double const internalReflection{ 1 - (n2 * n2) * (1 - (cosTheta * cosTheta)) };

		//If this is negative we have total internal reflection. (Sqr root would be imaginary)
		//This seems to be the same criteria that the incidence angle must fulfill the Brewsters angle.
		if (internalReflection < 0.0) {
			//std::cout << "TIR " << std::to_string(glm::acos(cosTheta)) << '\n';
			return nullptr;
		}
		//Make sure the incidence angle fulfill Brewster angle. 
		//double BrewsterAngle{ glm::atan(n2 / n1) }; 
		//if (glm::acos(cosTheta) > BrewsterAngle) {
		//	return nullptr;
		//}
		
		//Not sure if I'm assuming something wrong, but the glm::refract function gives wierd refraction rays
		//when used from inside the object. There is a difference (negative sign on inRay) in how it computes the dot product compared to above.
		//glm::dvec3 refractedDir{ glm::refract(inRay.rayDirection(), surfaceNormal, n1 / n2) };
		refractedDir = n2 * inRay.rayDirection() + surfaceNormal * (-n2 * cosTheta - std::sqrt(internalReflection)) ;
	}

	refractedDir = glm::normalize(refractedDir);
	
	//Lecture 5 page 10
	double R0{ ((n1 - n2) / (n1 + n2)) * ((n1 - n2) / (n1 + n2)) };
	glm::dvec3 refractiveCoefficient{ 1.0 - glm::dvec3{ R0 + (1 - R0) * std::pow((1 - cosTheta), 5) } };
	glm::dvec3 refractedImportance{ inRay.importance * refractiveCoefficient };//inRay.hitinfo->brdf().lambertianReflectionCoeff };

	Ray refractedRay{ inRay.intersectionPoint - surfaceNormal * EPS, refractedDir, refractedImportance };

	return new Node{ refractedRay };
}

/*
Estimator for lambertian reflection:
PDF used is 1/(pi^2)
We draw uniform values for x_i and y_i between 0 and 1 and map them
so that phi_i = 2PIx_i and theta_i = PIy_i/2 
*/
//Based on lecture 6 - slide 21/22, lecture 9 - slide 14-18, lecture 10 - slide 22
RayTree::Node* RayTree::computeDiffiuseReflection(Ray const& inRay) {
	//Get the random azimuth and inclination angle for the random out direction
	double x{ _rho_distribution(_generator) };
	double y{ _theta_distribution(_generator) };
	assert(x > 0.0 && x < 1.0);
	assert(y > 0.0 && y < 1.0);
	//rho
	double azimuth{ 2.0 * M_PI * x };
	//theta
	double inclination{ M_PI_2 * y };

	//Terminate lambertian rays using Russian roulette
	if (russianRoulette(inRay, x))
		return nullptr;

	//Compute local to world transformation matrix using the in ray
	glm::dmat4 M_L2W{ localToWorld(inRay) };
	//Compute the ray direction in local coordinate system
	glm::dvec3 localOutRayDirection{ std::move(SphericalToCartesian(azimuth, inclination)) };
	//Transform to world coordinates
	glm::dvec3 worldOutRayDirection{ M_L2W * glm::dvec4{localOutRayDirection, 1.0} };
	//Compute importance, lecture 9 slide 16
	//W_child = W_parent * PI * rho * cos(theta_out) * sin(theta_out)
	glm::dvec3 const& sigma{ inRay.hitinfo->brdf().lambertianReflectionCoeff };
	glm::dvec3 F_theta{ M_PI * sigma * inRay.hitinfo->color() * std::cos(inclination) * std::sin(inclination) };
	glm::dvec3 alpha{ 1.0 - sigma };
	//Compensate importance with 1/(1-alpha), alpha = 1 - sigma
	//This is because of russian roulette, see lecture 10 slide 23
	glm::dvec3 rayImportance{ F_theta / (1.0 - alpha) * inRay.importance  };

	Ray reflectedRay{ inRay.intersectionPoint, worldOutRayDirection, rayImportance };

	return new Node{ reflectedRay };
}

bool RayTree::russianRoulette(Ray const& inRay, double randomValue) const {
	//if randomvalue is between 0 and 1-alpha we keep the value otherwise discard
	//if alpha is 1-sigma and sigma is 1 we get 1-(1-1) = 1 so 100% that we will keep the values
	//if sigma is 0.8 we get 1-(1-0.8) = 1-0.2 = 0.8
	double sigma{ inRay.hitinfo->brdf().lambertianReflectionCoeff.x };
	double alpha{ 1.0 - sigma };
	return randomValue >= 1.0 - alpha; //this folds to  >= sigma...
}

glm::dmat4 RayTree::worldToLocal(Ray const& inRay) const {
	//Compute X,Y,Z coordinate axis, 
	//Z is the surface normal at the intersection point
	//X is the orthogonal direction to Z, computed using the ray direction 
	//Y is computed as the cross product
	glm::dvec3 Z{ inRay.hitinfo->getNormal(inRay.intersectionPoint) };
	glm::dvec3 I{ inRay.direction };
	glm::dvec3 I_orthogonal{ I - (glm::dot(I, Z) * Z) };
	glm::dvec3 X{ glm::normalize(I_orthogonal) };
	glm::dvec3 Y{ glm::normalize(glm::cross(-X, Z)) };

	glm::dmat4 T{ glm::translate(glm::dmat4{1.0}, -inRay.intersectionPoint) };
	glm::dmat4 R{
		glm::dvec4{ X, 0.0 },
		glm::dvec4{ Y, 0.0 },
		glm::dvec4{ Z, 0.0 },
		glm::dvec4{ 0.0, 0.0, 0.0, 1.0 }
	};
	//No idea why M = R * T does not work, but transposing seem to do the trick;
	return glm::transpose(R) * glm::transpose(T);
}

glm::dmat4 RayTree::localToWorld(Ray const& inRay) const {
	return glm::inverse(worldToLocal(inRay));
}

glm::dvec3 RayTree::SphericalToCartesian(double rho, double theta) const {
	return glm::dvec3{
	    std::cos(rho) * std::sin(theta),
		std::sin(rho) * std::sin(theta),
		std::cos(theta)
	};
}