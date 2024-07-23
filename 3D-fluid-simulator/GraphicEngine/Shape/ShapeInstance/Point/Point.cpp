#include <Point.h>

Point::Point(Vertex center, float radius)
	:Shape<GL_POINTS>(VertexArray({ center }, { 0 }), radius, false)
{
}

Point::~Point()
{}

