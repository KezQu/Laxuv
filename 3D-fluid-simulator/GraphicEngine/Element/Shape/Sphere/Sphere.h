#pragma once

#include <Shape.h>

class Sphere : public Shape {
private:
	static constexpr float radiusScaler = 1.90211303259030714423287866675876428;
public:
	Sphere();
	Sphere(Vertex center, float radius = 150, bool enableTess = false);
	virtual ~Sphere();
};
