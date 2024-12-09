#include "Line.h"

Line::Line(Vertex v1, Vertex v2)
	:Shape<GL_LINES>(VertexArray{ {v1, v2}, {0, 1} }, glm::distance(v1.coordinate, v2.coordinate), false)
{}
