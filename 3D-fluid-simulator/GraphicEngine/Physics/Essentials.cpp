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
  out << "(" << particle.positionGroup.x << "," << particle.positionGroup.y
      << "," << particle.positionGroup.z << "," << particle.positionGroup.w
      << ")" << std::endl;
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