#include "Point.h"

#include "Essentials.h"
#include "GL/glew.h"
#include "Shape.h"

Point::Point(Vertex center, float radius)
    : Shape<GL_POINTS>(VertexArray({center}, {0}), radius, false)
{
}

Essentials::DistributionShape Point::GetParticleDistribution()
{
  return Essentials::DistributionShape::SPHERE;
}
