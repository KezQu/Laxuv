#pragma once

#include <Shape.h>

class Square : public Shape {
public:
	Square();
	Square(Vertex center, float edgeLength= 300);
	Square(Vertex v0, Vertex v1, Vertex v2, Vertex v3);
	virtual ~Square();
};