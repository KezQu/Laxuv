#include "Line.h"

Line::Line(Vertex v1, Vertex v2)
	:Object(VertexArray{ {v1, v2}, {0, 1} })
{
	_center = v1.coordinate + v2.coordinate;
}
