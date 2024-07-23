#pragma once

#include <Shape.h>

class Square : public Shape<GL_TRIANGLES> {
public:
	Square(Vertex center = {{0,0,0}}, float radius = 150);
	Square(Square const& obj_copy) = delete;
	Square(Square&& obj_move) = default;
	Square& operator=(Square const& obj_copy) = delete;
	Square& operator=(Square&& obj_move) = default;
	virtual ~Square();
protected:
	Square(Vertex v0, Vertex v1, Vertex v2, Vertex v3, int radius);
};