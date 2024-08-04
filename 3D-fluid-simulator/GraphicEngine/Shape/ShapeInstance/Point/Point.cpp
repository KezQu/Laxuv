#include <Point.h>

Point::Point(Vertex center, uint64_t radius)
	:Shape<GL_POINTS>(VertexArray({ center }, { 0 }), radius, false)
{
}

DistributionShape Point::GetParticleDistribution()
{
	return DistributionShape::SPHERE;
}
