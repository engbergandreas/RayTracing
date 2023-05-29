#ifndef UTILS_H
#define UTILS_H

#include <glm/glm.hpp>
#define _USE_MATH_DEFINES
#include <math.h>

namespace color {
    constexpr glm::dvec3 RED{ 1.0, 0.01, 0.01 };
    constexpr glm::dvec3 GREEN{ 0.01, 1.0, 0.0 };
    constexpr glm::dvec3 BLUE{ 0.01, 0.01, 1.0 };
    constexpr glm::dvec3 WHITE{ 1.0, 1.0, 1.0 };
    constexpr glm::dvec3 YELLOW{ 1.0, 1.0, 0.01 };
    constexpr glm::dvec3 PURPLE{ 1.0, 0.0, 1.0 };
    constexpr glm::dvec3 CYAN{ 0.01, 1.0, 1.0 };
    constexpr glm::dvec3 PINK{ 1.0, 0.01, 0.5 };
    constexpr glm::dvec3 ORANGE{ 1.0, 0.5, 0.01 };
    constexpr glm::dvec3 BROWN{ 0.5, 0.25, 0.01 };
    constexpr glm::dvec3 GREY{ 0.5, 0.5, 0.5 };
} //namespace color

namespace settings {
    //Camera settings
    constexpr int CAMERA_PLANE_WIDTH{ 800 };
    constexpr int CAMERA_PLANE_HEIGHT{ 800 };
    constexpr double DELTA_WIDTH{ 2.0 / CAMERA_PLANE_WIDTH };
    constexpr double DELTA_HEIGHT{ 2.0 / CAMERA_PLANE_HEIGHT };
    //Render settings
    constexpr int SUPERSAMPLING{ 16 };
    constexpr int MAX_RAY_DEPTH{ 20 };
    constexpr int MAX_TRANSPARENT_DEPTH{ 4 };
    constexpr size_t NUMBER_OF_SHADOW_RAYS{ 10 };
    /*If multiple diffuse reflections are used the number of rays
    * will scale with n = sum_k(2^k), k = 0,1,...,MAX_RAY_DEPTH.
    * e.g., max depth of 10 gives rougly 2k rays per subpixel.
    */
    constexpr bool MULTIPLE_DIFFUSE_REFLECTIONS{ false };
    constexpr double LOCAL_DIFFUSE_LIGHT_COEFFICIENT{ 0.1 };
    //Render settings that are not really going to be changed
    constexpr double IMPORTANCE_THRESHOLD{ 1e-5 };

    static const char* FILENAME{ "../images/test.bmp" };
    constexpr bool USE_MULTI_THREADING{ true };
} // namespace settings

namespace constants {
    constexpr double IOR_AIR{ 1.0 };
    constexpr double EPSILON{ 1e-7 };
    constexpr double NOT_FOUND{ -1.0 };
} //namespace constants
#endif // !UTILS_H
