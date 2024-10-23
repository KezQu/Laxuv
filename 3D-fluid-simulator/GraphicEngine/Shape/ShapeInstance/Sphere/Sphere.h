#pragma once

#include "Essentials.h"
#include "GL/glew.h"
#include "Shape.h"
#include "VertexArray.h"

class Sphere : public Shape<GL_TRIANGLES>
{
 private:
  static constexpr float radiusScaler = 1.90211303259030714423287866675876428;

 public:
  Sphere(Vertex center = {{0, 0, 0}}, float radius = 15.f,
         bool enableTess = true);
  Sphere(Sphere const& obj_copy) = delete;
  Sphere(Sphere&& obj_move) = default;
  Sphere& operator=(Sphere const& obj_copy) = delete;
  Sphere& operator=(Sphere&& obj_move) = default;
  ~Sphere() override = default;
  Essentials::DistributionShape GetParticleDistribution() override;
};
