#pragma once

#include <Object.h>

class Sphere : public Object<GL_TRIANGLES> {
public:
	Sphere();
	Sphere(Vertex center, float radius = 150);
};
