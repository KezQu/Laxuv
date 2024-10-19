#pragma once

#include <Uniform.h>

#include <cstdint>
#include <glm/vec4.hpp>
#include <ostream>

namespace Essentials
{

constexpr uint32_t MaxNeighbours = 512;

enum class DistributionShape : uint8_t
{
  UNDEFINED,
  LINE,
  CIRCLE,
  SQUARE,
  DISK,
  QUBE,
  SPHERE
};

enum class SimulationState : uint8_t
{
  IDLE,
  INIT,
  SIMULATION,
  GEN_FRAME
};

enum class PhysicsType : uint8_t
{
  NONE,
  STATIC,
  DYNAMIC
};

struct ParticleProperties
{
  glm::vec4 velocityDFSPHfactor{0};
  glm::vec4 position{0};
  glm::vec4 VolumeDensityPressureRohash{0};
  uint32_t neighbours[MaxNeighbours];
};

struct FluidProperties
{
  uint32_t mesh_radius{1U};
  Uniform<float> mass{18.f, "mass", 1e-3f};
  Uniform<float> viscosity_factor{10.f, "viscosityFactor", 1e-3f};
  Uniform<float> density0{18.f, "density0", 1e-3f};
  Uniform<float> influence_kernel{2.5f, "influenceKernel"};
  Uniform<float> search_kernel{2.5f, "searchKernel"};
  Uniform<float> kernel_a{6.f, "kernel_a"};
  Uniform<float> particle_radius{0U, "particleRadius"};
  Uniform<float> particle_spacing{1.5f, "particleSpacing"};
  Uniform<uint8_t> distribution_shape{
      static_cast<uint8_t>(DistributionShape::QUBE), "DistributionShape"};
};

}  // namespace Essentials

std::ostream& operator<<(std::ostream& out,
                         Essentials::ParticleProperties const& particle);