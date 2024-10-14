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
  glm::vec4 positionGroup{0};
  glm::vec4 VolumeDensityPressureRohash{0};
  uint32_t neighbours[MaxNeighbours];
};

struct FluidProperties
{
  uint32_t mesh_radius{1U};
  Uniform<float> mass{0.018f, "mass"};
  Uniform<float> viscosity_factor{0.01f, "viscosityFactor"};
  Uniform<float> density0{0.018f, "density0"};
  Uniform<float> influence_kernel{2.5f, "influenceKernel"};
  Uniform<float> search_kernel{32.f, "searchKernel"};
  Uniform<float> kernel_a{4.f, "kernel_a"};
  Uniform<float> particle_radius{0U, "particleRadius"};
  Uniform<float> particle_spacing{1.5f, "particleSpacing"};
  Uniform<float> space_limiter{100.f, "spaceLimiter"};
  Uniform<float> bounds_viscosity{0.95f, "boundsViscosity"};
  Uniform<uint8_t> distribution_shape{
      static_cast<uint8_t>(DistributionShape::QUBE), "DistributionShape"};
};

}  // namespace Essentials

std::ostream& operator<<(std::ostream& out,
                         Essentials::ParticleProperties const& particle);