#pragma once

#include <Shape.h>

class Qube : public Shape {
public:
	Qube();
	Qube(Vertex center, float edgeLength = 300, bool enableTess = false);
	Qube(Vertex v0, Vertex v1, Vertex v2, Vertex v3, Vertex v4, Vertex v5, Vertex v6, Vertex v7, bool enableTess = false);
	virtual ~Qube();
};
