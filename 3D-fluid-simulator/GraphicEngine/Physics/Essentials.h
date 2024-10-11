#pragma once

#include <Uniform.h>

#include <cstdint>
#include <glm/vec4.hpp>
#include <ostream>

namespace Essentials
{

constexpr uint32_t KernelRadius = 4;
constexpr uint32_t MaxParticles = 27;
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
  glm::vec4 VolumeDensityPressureMass{0};
  uint32_t neighbours[512];
};

struct FluidProperties
{
  uint32_t mesh_radius{1U};
  Uniform<float> mass{0.1f, "mass"};
  Uniform<float> eos_factor{50.f, "EOSFactor"};
  Uniform<float> viscosity_factor{0.2f, "viscosityFactor"};
  Uniform<float> density0{0.15f, "density0"};
  Uniform<uint32_t> influence_kernel{1U, "influenceKernel"};
  Uniform<uint32_t> search_kernel{8U, "searchKernel"};
  Uniform<uint32_t> particle_radius{0U, "particleRadius"};
  Uniform<uint32_t> particle_spacing{1U, "particleSpacing"};
  Uniform<uint32_t> space_limiter{100U, "spaceLimiter"};
  Uniform<float> bounds_viscosity{0.95f, "boundsViscosity"};
  Uniform<uint8_t> distribution_shape{
      static_cast<uint8_t>(DistributionShape::QUBE), "DistributionShape"};
};

}  // namespace Essentials

std::ostream& operator<<(std::ostream& out,
                         Essentials::ParticleProperties const& particle);