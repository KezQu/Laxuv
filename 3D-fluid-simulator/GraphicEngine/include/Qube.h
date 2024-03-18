#pragma once

#include <Object.h>

class Qube : public Object<GL_TRIANGLES> {
public:
	Qube();
	Qube(Vertex center, float edgeLength = 300);
	Qube(Vertex v0, Vertex v1, Vertex v2, Vertex v3, Vertex v4, Vertex v5, Vertex v6, Vertex v7);
};
