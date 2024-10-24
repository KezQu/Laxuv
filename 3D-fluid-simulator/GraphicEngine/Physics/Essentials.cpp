#include "Essentials.h"

#include <iostream>

std::ostream& operator<<(std::ostream& out,
                         Essentials::ParticleProperties const& particle)
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

std::string Essentials::DistShapeToString(DistributionShape shape) noexcept
{
  switch (shape)
  {
    case DistributionShape::LINE:
      return "Line";
      break;
    case DistributionShape::CIRCLE:
      return "Circle";
      break;
    case DistributionShape::SQUARE:
      return "Square";
      break;
    case DistributionShape::DISK:
      return "Disk";
      break;
    case DistributionShape::CUBE:
      return "Cube";
      break;
    case DistributionShape::SPHERE:
      return "Sphere";
      break;
    default:
      return "Undefined";
      break;
  }
}

char const* Essentials::DistShapeTolist() noexcept
{
  return " \0Point\0Line\0Square\0Cube\0Sphere\0";
}

char const* Essentials::WorldTypeTolist() noexcept
{
  return " \0Cube\0Sphere\0";
}

std::string Essentials::PhysTypeToString(PhysicsType physics) noexcept
{
  switch (physics)
  {
    case PhysicsType::STATIC:
      return "Static";
      break;
    case PhysicsType::DYNAMIC:
      return "Dynamic";
      break;
    default:
      return "Undefined";
      break;
  }
}

char const* Essentials::PhysTypesTolist() noexcept
{
  return "Undefined\0Static\0Dynamic\0";
}

char const* Essentials::ColorPropertyTolist() noexcept
{
  return " \0Velocity\0Density error\0Divergence error\0Pressure\0";
}
