#pragma once

#include <Object.h>

class Sphere : public Object<GL_PATCHES> {
public:
	Sphere();
	Sphere(Vertex center, float radius = 150);
	void BindUniforms() const override;
};
