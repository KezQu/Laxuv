#pragma once

#include <Shape.h>

class Cube : public Shape<GL_TRIANGLES>
{
 public:
  Cube(Vertex center = {{0, 0, 0}}, float radius = 15.f,
       bool enableTess = false);
  Cube(Cube const& obj_copy) = delete;
  Cube(Cube&& obj_move) = default;
  Cube& operator=(Cube const& obj_copy) = delete;
  Cube& operator=(Cube&& obj_move) = default;
  ~Cube() override = default;
  Essentials::DistributionShape GetParticleDistribution() override;

 protected:
  Cube(Vertex v0, Vertex v1, Vertex v2, Vertex v3, Vertex v4, Vertex v5,
       Vertex v6, Vertex v7, float radius, bool enableTess = false);
};
