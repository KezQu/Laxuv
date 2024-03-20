#pragma once

#include <Object.h>

class Square : public Object<GL_TRIANGLES> {
public:
	Square();
	Square(Vertex center, float edgeLength= 300);
	Square(Vertex v0, Vertex v1, Vertex v2, Vertex v3);
};