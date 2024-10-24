#pragma once

#include <Uniform.h>

#include <cstdint>
#include <glm/vec4.hpp>

namespace Essentials
{

constexpr float cellRadius = 5;
constexpr uint64_t MaxCells = static_cast<uint64_t>(1200 / cellRadius);
constexpr uint64_t CellCapacity = static_cast<uint64_t>(
    3.1415 * (4. / 3.) * cellRadius * cellRadius * cellRadius);
constexpr uint32_t MaxNeighbours = 512;

using space_grid_t = uint32_t[CellCapacity];

enum class DistributionShape : uint32_t
{
  UNDEFINED,
  LINE,
  CIRCLE,
  SQUARE,
  DISK,
  CUBE,
  SPHERE
};
std::string DistShapeToString(DistributionShape shape) noexcept;
char const* DistShapeTolist() noexcept;

enum class SimulationState : uint32_t
{
  IDLE,
  INIT,
  SIMULATION,
  GEN_FRAME
};

enum class WorldType : uint32_t
{
  NONE,
  CUBE_WORLD,
  SHPERE_WORLD
};
char const* WorldTypeTolist() noexcept;

enum class PhysicsType : uint32_t
{
  NONE,
  STATIC,
  DYNAMIC
};
std::string PhysTypeToString(PhysicsType physics) noexcept;
char const* PhysTypesTolist() noexcept;

enum class ColorProperty : uint32_t
{
  NONE,
  VELOCITY,
  DENSITY_ERROR,
  DIVERGENCE_ERROR,
  PRESSURE,
};
char const* ColorPropertyTolist() noexcept;

struct ParticleProperties
{
  glm::vec4 velocityDFSPHfactor{0};
  glm::vec4 position{0};
  glm::vec4 VolumeDensityPressureRohash{0};
  glm::vec4 particleColor{0};
  uint32_t neighbours[MaxNeighbours];
};

auto const lengthDefaultProperties =
    ValueProperties{1.f, 600.f, 1.f, "%.1f mm"};

struct FluidProperties
{
  Uniform<float> particle_radius{0.f, "shapeRadius", lengthDefaultProperties};
  Uniform<float> particle_spacing{1.f, "particleSpacing",
                                  lengthDefaultProperties};
  Uniform<uint32_t> distribution_shape{
      static_cast<uint32_t>(DistributionShape::CUBE), "DistributionShape"};
  Uniform<float> influence_kernel{2.5f, "influenceKernel",
                                  lengthDefaultProperties};
  Uniform<float> search_kernel{2.5f, "searchKernel", lengthDefaultProperties};
  Uniform<float> kernel_a{4.f, "kernel_a"};
  Uniform<float> mass{1.f, "mass",
                      ValueProperties{0.1f, 1000.f, 1e-3f, "%.1f g"}};
  Uniform<float> viscosity_factor{
      10.f, "viscosityFactor",
      ValueProperties{0.1f, 1000.f, 1e-3f, "%.1f Pa*s"}};
  uint32_t mesh_radius{1U};
};

}  // namespace Essentials

std::ostream& operator<<(std::ostream& out,
                         Essentials::ParticleProperties const& particle);