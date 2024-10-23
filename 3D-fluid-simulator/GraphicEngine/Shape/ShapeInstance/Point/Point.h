#pragma once

#include "Essentials.h"
#include "GL/glew.h"
#include "Shape.h"
#include "VertexArray.h"

class Point : public Shape<GL_POINTS>
{
 public:
  Point(Vertex center = {{0, 0, 0}}, float radius = 15.f);
  Point(Point const& obj_copy) = delete;
  Point(Point&& obj_move) = default;
  Point& operator=(Point const& obj_copy) = delete;
  Point& operator=(Point&& obj_move) = default;
  ~Point() override = default;
  Essentials::DistributionShape GetParticleDistribution() override;
};
