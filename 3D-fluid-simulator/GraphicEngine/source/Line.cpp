#include "Line.h"

Line::Line(Vertex p1, Vertex p2)
	:Object<GL_LINES>(Program{
		{ GL_VERTEX_SHADER, "\\shaders\\CalculateNDC.vert" },
		{ GL_VERTEX_SHADER, "\\shaders\\Object.vert" },
		{ GL_FRAGMENT_SHADER, "\\shaders\\Object.frag" } },
		VertexArray{ {p1, p2}, {0, 1} })
{}
