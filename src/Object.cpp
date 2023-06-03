#include "Object.h"

void Object::transformObject(glm::dmat4x4 const& M)
{
	for (Triangle& t : sides) {
		t.transformTriangle(M);
	}
}

Tetrahedron::Tetrahedron(glm::dvec3 const& color, BRDF const& brdf)
{
    Triangle t0(glm::dvec3{ 0.0, 0.0, 0.0 }, glm::dvec3{ 0.0, 1.0, 0.0 }, glm::dvec3{ 1.0, 0.0, 0.0 }, color, brdf);
    Triangle t1(glm::dvec3{ 0.0, 0.0, 0.0 }, glm::dvec3{ 0.5, 0.5, 1.0 }, glm::dvec3{ 0.0, 1.0, 0.0 }, color, brdf);
    Triangle t2(glm::dvec3{ 1.0, 0.0, 0.0 }, glm::dvec3{ 0.0, 1.0, 0.0 }, glm::dvec3{ 0.5, 0.5, 1.0 }, color, brdf);
    Triangle t3(glm::dvec3{ 0.0, 0.0, 0.0 }, glm::dvec3{ 1.0, 0.0, 0.0 }, glm::dvec3{ 0.5, 0.5, 1.0 }, color, brdf);

    sides.push_back(t0);
    sides.push_back(t1);
    sides.push_back(t2);
    sides.push_back(t3);
}

Cube::Cube(glm::dvec3 const& color, BRDF const& brdf) : Object{}
{
    // Cube from: https://pastebin.com/XiCprv6S
    glm::dvec3 vertices[] = {
        glm::dvec3{ -0.5,  -0.5,  -0.5 },   // A 0
        glm::dvec3{ 0.5, -0.5, -0.5 },  // B 1
        glm::dvec3{ 0.5,  0.5, -0.5 },  // C 2
        glm::dvec3{ -0.5, 0.5,  -0.5 },  // D 3
        glm::dvec3{ -0.5, -0.5,   0.5 },    // E 4
        glm::dvec3{ 0.5, -0.5,  0.5 }, // F 5
        glm::dvec3{ 0.5, 0.5,  0.5 }, // G 6
        glm::dvec3{ -0.5,   0.5,   0.5 },  // H 7

        glm::dvec3{ -0.5,  0.5, -0.5 },  // D 8
        glm::dvec3{ -0.5, -0.5, -0.5 },  // A 9
        glm::dvec3{ -0.5, -0.5,  0.5 },   // E 10
        glm::dvec3{ -0.5,  0.5,  0.5 },  // H 11
        glm::dvec3{ 0.5, -0.5, -0.5 },    // B 12
        glm::dvec3{ 0.5,  0.5, -0.5 },  // C 13
        glm::dvec3{ 0.5,  0.5,  0.5 },   // G 14
        glm::dvec3{ 0.5, -0.5,  0.5 },   // F 15

        glm::dvec3{ -0.5, -0.5, -0.5 },  // A 16
        glm::dvec3{ 0.5, -0.5, -0.5 },  // B 17
        glm::dvec3{ 0.5, -0.5,  0.5 },   // F 18
        glm::dvec3{ -0.5, -0.5,  0.5 },  // E 19
        glm::dvec3{ 0.5,  0.5, -0.5 },  // C 20
        glm::dvec3{ -0.5,  0.5, -0.5 },  // D 21
        glm::dvec3{ -0.5,  0.5,  0.5 },   // H 22
        glm::dvec3{ 0.5,  0.5,  0.5 },   // G 23
    };

    sides = {
        // front and back
        Triangle{ vertices[0], vertices[3], vertices[2], color, brdf },
        Triangle{ vertices[2], vertices[1], vertices[0], color, brdf },
        Triangle{ vertices[4], vertices[5], vertices[6], color, brdf },
        Triangle{ vertices[6], vertices[7], vertices[4], color, brdf },
        // left and right
        Triangle{ vertices[11], vertices[8], vertices[9], color, brdf },
        Triangle{ vertices[9], vertices[10], vertices[11], color, brdf },
        Triangle{ vertices[12], vertices[13], vertices[14], color, brdf },
        Triangle{ vertices[14], vertices[15], vertices[12], color, brdf },
        // bottom and top
        Triangle{ vertices[16], vertices[17], vertices[18], color, brdf },
        Triangle{ vertices[18], vertices[19], vertices[16], color, brdf },
        Triangle{ vertices[20], vertices[21], vertices[22], color, brdf },
        Triangle{ vertices[22], vertices[23], vertices[20], color, brdf }
    };
}