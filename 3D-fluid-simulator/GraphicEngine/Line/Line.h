#pragma once

#include <Object.h>

class Line : public Object<GL_LINES>{
public:
	Line(Vertex p1, Vertex p2);
};
