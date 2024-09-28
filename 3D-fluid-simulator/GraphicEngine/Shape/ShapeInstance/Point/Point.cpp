#include <Point.h>

Point::Point(Vertex center, uint64_t radius)
	:Shape<GL_POINTS>(VertexArray({ center }, { 0 }), radius, false)
{
}

Essentials::DistributionShape Point::GetParticleDistribution()
{
	return Essentials::DistributionShape::SPHERE;
}
