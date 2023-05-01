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

	//Exit early if we hit a diffuse surface. 
	if (ptr->_ray.hitinfo->brdf().isDiffuse())
		return;

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
	glm::dvec3 radiance = { (reflectedRadiance * reflectedImportance + refractedRadiance * refractedImportance) / ptr->_ray.importance +
		localRadiance * LD_coeff };

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
	