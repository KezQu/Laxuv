#include "Line.h"

Line::Line(Vertex v1, Vertex v2)
	:Element(VertexArray{ {v1, v2}, {0, 1} })
{}

Line::~Line() 
{}