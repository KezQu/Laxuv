#include "Essentials.h"

#include <iostream>

std::ostream& operator<<(std::ostream& out,
                         Essentials::ParticleBufferProperties const& particle)
{
  out << "- - - - - - - -" << std::endl;
  // out << "(" << particle.force.x << "," << particle.force.y << ","
  //     << particle.force.z << "," << particle.force.w << ")" << std::endl;
  out << "(" << particle.velocityDFSPHfactor.x << ","
      << particle.velocityDFSPHfactor.y << "," << particle.velocityDFSPHfactor.z
      << "," << particle.velocityDFSPHfactor.w << ")" << std::endl;
  out << "(" << particle.position.x << "," << particle.position.y << ","
      << particle.position.z << "," << particle.position.w << ")" << std::endl;
  out << "(" << particle.VolumeDensityPressureRohash.x << ","
      << particle.VolumeDensityPressureRohash.y << ","
      << particle.VolumeDensityPressureRohash.z << ","
      << particle.VolumeDensityPressureRohash.w << ")" << std::endl;
  out << "[";
  for (auto& neighbour_id : particle.neighbours)
  {
    out << neighbour_id << ", ";
  }
  out << "]" << std::endl;
  out << "- - - - - - - -" << std::endl;
  return out;
}

char const* Essentials::EntityTypeTolist() noexcept
{
  return " \0OBJECT\0PARTICLES\0";
}

char const* Essentials::EntityShapeTolist() noexcept
{
  return " \0POINT\0LINE\0SQUARE\0CUBE\0SPHERE\0";
}

char const* Essentials::DistShapeTolist() noexcept
{
  return "UNDEFINED\0LINE\0CIRCLE\0SQUARE\0DISK\0CUBE\0SPHERE\0";
}

char const* Essentials::WorldTypeTolist() noexcept
{
  return " \0CUBE\0SHPERE\0";
}

char const* Essentials::PhysTypesTolist() noexcept
{
  return "NONE\0STATIC\0DYNAMIC\0";
}

char const* Essentials::ColorPropertyTolist() noexcept
{
  return "NONE\0VELOCITY\0DENSITY_ERROR\0DIVERGENCE_ERROR\0PRESSURE\0";
}

Uniform<glm::mat4, float> Essentials::ShapeProperties::Model() const
{
  auto I = glm::identity<glm::mat4>();
  auto rotation = _rotation.GetValue();
  auto scale = _scale.GetValue() * _radius.GetValue();
  auto location = _location.GetValue();
  auto angle = glm::max(glm::max(rotation.x, rotation.y), rotation.z);
  auto T =
      glm::dot(location, location) != 0.f ? glm::translate(I, location) : I;
  auto R = glm::dot(rotation, rotation) != 0.f
               ? glm::rotate(I, glm::radians(angle), glm::normalize(rotation))
               : I;
  auto S = glm::dot(scale, scale) != 0.f ? glm::scale(I, scale) : I;
  return Uniform<glm::mat4, float>{T * R * S, "model"};
}