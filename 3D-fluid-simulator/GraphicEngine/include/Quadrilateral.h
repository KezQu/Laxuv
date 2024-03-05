#pragma once

#include <Object.h>

class Quadrilateral : public Object {
public:
	Quadrilateral();
	Quadrilateral(Vertex v1, Vertex v2, Vertex v3, Vertex v4);
};