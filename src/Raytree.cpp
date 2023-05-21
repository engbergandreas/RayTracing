#include "Raytree.h"

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

	double const IMPORTANCE_THRESHOLD{ 1e-5 };

	//Exit early if we have reached max depth
	if (depth > _maxDepth)
		return;

	//Exit early if ray contribution will not improve image quality
	if (ptr->_ray.importance.r < IMPORTANCE_THRESHOLD &&
		ptr->_ray.importance.g < IMPORTANCE_THRESHOLD &&
		ptr->_ray.importance.b < IMPORTANCE_THRESHOLD)
		return;

	_scene.shootRayIntoScene(ptr->_ray);

	//Ray did not hit anything, ideally it should not happen (closed room).
	if (!ptr->_ray.hitinfo)
		return;

	//Ray intersect diffuse surface, cast reflection ray in random direction
	if (ptr->_ray.hitinfo->brdf().isDiffuse())
	{
		Node* diffuseReflection{ computeDiffiuseReflection(ptr->_ray) };
		diffuseReflection->_parent = ptr;
		ptr->_reflected = diffuseReflection;
		++depth;
		buildRayTree(ptr->_reflected, depth);
		return;
	}

	//Compute reflection ray direction
	Node* reflectedRay{ computeReflectedRay(ptr->_ray) };
	reflectedRay->_parent = ptr;
	ptr->_reflected = reflectedRay;

	++depth;

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
			//Compute shadow ray which introduce radiance to the tree. Only for diffuse surfaces
			if (ptr->_ray.hitinfo->brdf().isDiffuse())
				return shootShadowRay(ptr);
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

	//Compute local contribution at every intersection by shooting shadow rays.
	//Don't shoot shadow rays if we are at a transparent or specular object. 
	glm::dvec3 localRadiance{ ptr->_ray.hitinfo->brdf().isDiffuse() ? shootShadowRay(ptr) : glm::dvec3{0.0, 0.0, 0.0} };
	double LD_coeff{ 0.1 };

	//Radiance of parent is the combined radiance from reflected and refracted rays
	//Plus that of the local illumination model
	//Due to color bleeding between diffuse surfaces, the importance can become 0 in one of the channels
	//This avoid division by zero if that is the case. 
	glm::dvec3 radiance{ 0.0 };
	if (ptr->_ray.importance.r > 0.0)
		radiance.r = (reflectedRadiance.r * reflectedImportance.r + refractedRadiance.r * refractedImportance.r) / ptr->_ray.importance.r;
	if (ptr->_ray.importance.g > 0.0)
		radiance.g = (reflectedRadiance.g * reflectedImportance.g + refractedRadiance.g * refractedImportance.g) / ptr->_ray.importance.g;
	if (ptr->_ray.importance.b > 0.0)
		radiance.b = (reflectedRadiance.b * reflectedImportance.b + refractedRadiance.b * refractedImportance.b) / ptr->_ray.importance.b;

	//glm::dvec3 radiance = { (reflectedRadiance * reflectedImportance + refractedRadiance * refractedImportance) / ptr->_ray.importance +
	radiance += localRadiance * LD_coeff;

	return radiance;
}

glm::dvec3 RayTree::shootShadowRay(Node* ptr) {
	
	Lightsource light{ _scene.getLightSources().back()};
	glm::dvec3 intersectionPoint{ ptr->_ray.intersectionPoint };
	glm::dvec3 sRayEndPoint{ light.position };
	glm::dvec3 sRayDir{ sRayEndPoint - intersectionPoint };

	Ray shadowRay{ intersectionPoint, sRayDir, glm::dvec3{ 0.0 } };

	//Check if shadow ray is intersecting another object before reaching light source
	std::vector<Intersectable*> const& objects{ _scene.getObjects() };
	for (Intersectable const*  obj : objects) {
		//Simplification, shadowrays ignore transparent objects.
		if (obj->brdf()._material == Material::transparent)
			continue;

		double t = obj->rayIntersection(shadowRay);
		if (t > 0.0 && t < (1.0 - EPS)) {
			return glm::dvec3{ 0.0, 0.0, 0.0 }; //Point is in shadow of another object, no light contribution.
		}
	}

	double r{ glm::length(shadowRay.rayDirection(false)) };

	double cosTheta{ glm::dot(shadowRay.rayDirection(), ptr->_ray.hitinfo->getNormal(intersectionPoint)) };
	cosTheta = glm::clamp(cosTheta, 0.0, 1.0);
	
	double Sigma{ light.cross_section / (r * r) };
	glm::dvec3 irradiance{ Sigma * cosTheta * light.L0 };

	glm::dvec3 rho{ ptr->_ray.hitinfo->brdf().lambertianReflectionCoeff };
	glm::dvec3 emittedLight{ rho * irradiance / 3.14159 };

	return ptr->_ray.hitinfo->color() * emittedLight;
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
		double n1{ 1.0 }; //ior air
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
	double azimuth{ 2.0 * static_cast<double>(std::_Pi) * x }; //rho
	double inclination{ static_cast<double>(std::_Pi) * y / 2.0 }; //theta

	//Compute local to world transformation matrix using the in ray
	glm::dmat4 M_L2W{ localToWorld(inRay) };
	//Compute the ray direction in local coordinate system
	glm::dvec3 localOutRayDirection{ std::move(SphericalToCartesian(azimuth, inclination)) };
	//Transform to world coordinates
	glm::dvec3 worldOutRayDirection{ M_L2W * glm::dvec4{localOutRayDirection, 1.0} };
	//Compute importance, lecture 9 slide 16
	//W_child = W_parent * PI * rho * cos(theta_out) * sin(theta_out)
	glm::dvec3 rayImportance{ static_cast<double>(std::_Pi) * inRay.hitinfo->brdf().lambertianReflectionCoeff *
		inRay.hitinfo->color() * std::cos(inclination) * std::sin(inclination) * inRay.importance };

	//rayImportance = glm::clamp(rayImportance, glm::dvec3{ 0.0 }, glm::dvec3{ 1.0 });

	Ray reflectedRay{ inRay.intersectionPoint, worldOutRayDirection, rayImportance };

	return new Node{ reflectedRay };
	//Apply russian roulette?
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