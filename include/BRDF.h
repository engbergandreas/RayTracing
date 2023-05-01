#ifndef BRDF_H_
#define BRDF_H_

enum class Material {
    lambertian,
    specular,
    transparent
};

class BRDF {
    public:
        BRDF(Material material, glm::dvec3 const& rho, double ior) : _material{ material }, lambertianReflectionCoeff { rho }, ior{ ior }{}

    bool isDiffuse() const { return _material == Material::lambertian; }

    glm::dvec3 lambertianReflectionCoeff; //Rho

    double ior; //index of refraction e.g., 1.0 for air and 1.5 for glass

    Material _material;
};

#endif // !BRDF_H_