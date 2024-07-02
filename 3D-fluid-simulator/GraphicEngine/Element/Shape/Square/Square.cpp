#include <Square.h>

Square::Square() 
	:Square(
		{ {-1.f, 1.f, 0.0f} },
		{ {1.f, 1.f, 0.0f} },
		{ {-1.f, -1.f, 0.0f} },
		{ {1.f, -1.f, 0.0f} })
{}
Square::Square(Vertex center, float edgeLength)
	:Square(
		{ {center.coordinate[0] - edgeLength / 2.f, center.coordinate[1] + edgeLength / 2.f, center.coordinate[2]} },
		{ {center.coordinate[0] + edgeLength / 2.f, center.coordinate[1] + edgeLength / 2.f, center.coordinate[2]} },
		{ {center.coordinate[0] - edgeLength / 2.f, center.coordinate[1] - edgeLength / 2.f, center.coordinate[2]} },
		{ {center.coordinate[0] + edgeLength / 2.f, center.coordinate[1] - edgeLength / 2.f, center.coordinate[2]} })
{}
Square::Square(Vertex v1, Vertex v2, Vertex v3, Vertex v4)
	:Shape(VertexArray{ {v1, v2, v3, v4}, {0, 1, 2, 1, 3, 2} })
{}
Square::~Square()
{}
