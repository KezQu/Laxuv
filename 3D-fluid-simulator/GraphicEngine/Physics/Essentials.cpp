#include "Essentials.h"

#include "Uniform.h"
#include "glm/fwd.hpp"

char const* Essentials::EntityTypeTolist() noexcept
{
  return " \0OBJECT\0PARTICLES\0";
}

char const* Essentials::EntityShapeTolist() noexcept
{
  return " \0CUBE\0SPHERE\0";
}

char const* Essentials::DistShapeTolist() noexcept
{
  return "UNDEFINED\0LINE\0SQUARE\0CUBE\0";
}

char const* Essentials::WorldTypeTolist() noexcept
{
  return " \0CUBE\0SHPERE\0";
}

char const* Essentials::PhysTypesTolist() noexcept
{
  return "NONE\0STATIC\0DYNAMIC\0";
}

char const* Essentials::ColorPropertyTolistParticles() noexcept
{
  return "NONE\0CUSTOM\0VELOCITY\0DENSITY_ERROR\0DIVERGENCE_ERROR\0PRESSURE\0";
}

char const* Essentials::ColorPropertyTolistObject() noexcept
{
  return "NONE\0CUSTOM\0";
}

Uniform<glm::mat4, float> Essentials::ShapeProperties::Model() const
{
  auto const I = glm::identity<glm::mat4>();
  auto const rotation = _rotation.GetValue();
  auto const location = _location.GetValue();
  auto T = glm::translate(I, location);
  auto Rx = glm::rotate(I, glm::radians(rotation.x), glm::vec3{1, 0, 0});
  auto Rxy = glm::rotate(Rx, glm::radians(rotation.y), glm::vec3{0, 1, 0});
  auto Rxyz = glm::rotate(Rxy, glm::radians(rotation.z), glm::vec3{0, 0, 1});

  return Uniform<glm::mat4, float>{T * Rxyz, "model"};
}