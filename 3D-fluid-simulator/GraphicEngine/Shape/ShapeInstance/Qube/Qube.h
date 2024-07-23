#pragma once

#include <Shape.h>

class Qube : public Shape<GL_TRIANGLES> {
public:
	Qube(Vertex center = { {0,0,0} }, float radius = 150, bool enableTess = true);
	Qube(Qube const& obj_copy) = delete;
	Qube(Qube&& obj_move) = default;
	Qube& operator=(Qube const& obj_copy) = delete;
	Qube& operator=(Qube&& obj_move) = default;
	virtual ~Qube();
protected:
	Qube(Vertex v0, Vertex v1, Vertex v2, Vertex v3, Vertex v4, Vertex v5, Vertex v6, Vertex v7, int radius, bool enableTess = false);
};
