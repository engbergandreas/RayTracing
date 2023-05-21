#ifndef UTILS_H
#define UTILS_H

#include <glm/glm.hpp>

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
}
#endif // !UTILS_H
