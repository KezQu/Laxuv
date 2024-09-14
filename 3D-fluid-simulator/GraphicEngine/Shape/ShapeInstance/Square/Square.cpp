#include <Square.h>

Square::Square(Vertex center, uint64_t radius)
	:Square(
		{ {center.coordinate[0] - 1.f, center.coordinate[1] + 1.f, center.coordinate[2]} },
		{ {center.coordinate[0] + 1.f, center.coordinate[1] + 1.f, center.coordinate[2]} },
		{ {center.coordinate[0] - 1.f, center.coordinate[1] - 1.f, center.coordinate[2]} },
		{ {center.coordinate[0] + 1.f, center.coordinate[1] - 1.f, center.coordinate[2]} }, radius)
{}
Square::Square(Vertex v1, Vertex v2, Vertex v3, Vertex v4, uint64_t radius)
	:Shape(VertexArray{ {v1, v2, v3, v4}, {0, 1, 2, 1, 3, 2} }, radius, false)
{}

Essentials::DistributionShape Square::GetParticleDistribution()
{
	return Essentials::DistributionShape::SQUARE;
}
