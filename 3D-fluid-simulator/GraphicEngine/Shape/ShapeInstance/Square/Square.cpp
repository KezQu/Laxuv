#include <Square.h>

Square::Square(Vertex center, float radius)
    // clang-format off
	:Square(
		{ {center.coordinate[0] - 1.f, center.coordinate[1] + 1.f, center.coordinate[2]} },
		{ {center.coordinate[0] + 1.f, center.coordinate[1] + 1.f, center.coordinate[2]} },
		{ {center.coordinate[0] - 1.f, center.coordinate[1] - 1.f, center.coordinate[2]} },
		{ {center.coordinate[0] + 1.f, center.coordinate[1] - 1.f, center.coordinate[2]} }, radius)
// clang-format on
{
}
Square::Square(Vertex v1, Vertex v2, Vertex v3, Vertex v4, float radius)
    : Shape(VertexArray{{v1, v2, v3, v4}, {0, 1, 2, 1, 3, 2}}, radius, false)
{
}
