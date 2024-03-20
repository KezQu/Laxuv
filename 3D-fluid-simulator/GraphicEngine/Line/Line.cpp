#include "Line.h"

Line::Line(Vertex p1, Vertex p2)
	:Object<GL_LINES>(Program{
		{ GL_VERTEX_SHADER, "/CalculateNDC.glsl" },
		{ GL_VERTEX_SHADER, "/Object/Object.vert" },
		{ GL_FRAGMENT_SHADER, "/Object/Object.frag" } },
		VertexArray{ {p1, p2}, {0, 1} })
{}
