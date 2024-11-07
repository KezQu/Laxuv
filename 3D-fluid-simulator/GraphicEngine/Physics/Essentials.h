#pragma once

#include <cstdint>
#include <utility>
#include <vector>

#include "Uniform.h"
#include "glm/fwd.hpp"

namespace Essentials
{

constexpr float cellRadius = 5;
constexpr uint64_t MaxCells = static_cast<uint64_t>(1200 / cellRadius);
constexpr uint64_t CellCapacity = static_cast<uint64_t>(
    3.1415 * (4. / 3.) * cellRadius * cellRadius * cellRadius);
constexpr uint32_t MaxNeighbours = 512;

using space_grid_t = uint32_t[CellCapacity];

enum class EntityType : uint32_t
{
  NONE,
  OBJECT,
  PARTICLES
};
char const* EntityTypeTolist() noexcept;

enum class EntityShape : uint32_t
{
  NONE,
  POINT,
  LINE,
  SQUARE,
  CUBE,
  SPHERE
};
char const* EntityShapeTolist() noexcept;

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
char const* DistShapeTolist() noexcept;

enum class SimulationState : uint32_t
{
  IDLE,
  INIT,
  SIMULATION,
  GEN_FRAME,
  GRAPHS
};

enum class WorldType : uint32_t
{
  NONE,
  CUBE_T,
  SHPERE_T
};
char const* WorldTypeTolist() noexcept;

enum class PhysicsType : uint32_t
{
  NONE,
  STATIC,
  DYNAMIC
};
char const* PhysTypesTolist() noexcept;

enum class ColorProperty : uint32_t
{
  NONE,
  CUSTOM,
  VELOCITY,
  DENSITY_ERROR,
  DIVERGENCE_ERROR,
  PRESSURE,
};
char const* ColorPropertyTolist() noexcept;

struct TerrainBufferProperties
{
  glm::vec4 center{0};
  glm::vec4 bounds{0};
  glm::mat4 model{0};
};

struct ParticleBufferProperties
{
  glm::vec4 velocityDFSPHfactor{0};
  glm::vec4 position{0};
  glm::vec4 VolumeDensityPressureDro_Dt{0};
  glm::vec4 color{0};
  uint32_t neighbours[MaxNeighbours];
};

auto const lengthDefaultProperties =
    ValueProperties{0.1f, 600.f, 1.f, "%.1f mm"};

auto const colorDefaultProperties = ValueProperties{0.f, 1.f, 1.f, "%.2f"};

struct Light
{
  Uniform<glm::vec3, float> ambient_color{glm::vec3{.2f}, "ambientColor",
                                          colorDefaultProperties};
  struct
  {
    Uniform<glm::vec3, float> color{glm::vec3{1.f}, "diffuseColor",
                                    colorDefaultProperties};
    Uniform<glm::vec3, float> direction{
        glm::vec3{-0.3f, -0.2f, -1.f}, "diffuseDirection",
        ValueProperties{-1.f, 1.f, 1.f, "%.2f"}};
  } diffuse;
};

struct ParticleProperties
{
  Uniform<glm::vec3, float> init_velocity{
      glm::vec3{0.f}, "initVelocity",
      ValueProperties{-50.f, 50.f, 1.f, "%.1f m/s"}};
  Uniform<float> particle_spacing{2.f, "particleSpacing",
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
};

struct ShapeProperties
{
  Uniform<glm::vec3, float> _location{
      glm::vec3{0.f}, "location",
      ValueProperties{-1200.f, 1200.f, 1.f, "%.1f mm"}};
  Uniform<glm::vec3, float> _scale{glm::vec3{1.f}, "scale",
                                   ValueProperties{0.f, 100.f, 1.f, "%.1f"}};
  Uniform<glm::vec3, float> _rotation{
      glm::vec3{0.f}, "rotation",
      ValueProperties{-360.f, 360.f, 1.f, "%.1f deg"}};
  Uniform<glm::vec3, float> _center{
      glm::vec3{0.f}, "center",
      ValueProperties{-1200.f, 1200.f, 1.f, "%.1f mm"}};
  Uniform<uint32_t> _subdivision{5U, "subdivision", ValueProperties{1U, 50U}};
  Uniform<float> _radius{1.f, "shapeRadius", lengthDefaultProperties};
  std::pair<Uniform<uint32_t>, Uniform<glm::vec4, float>> _color{
      {static_cast<uint32_t>(Essentials::ColorProperty::NONE), "colorType"},
      {glm::vec4{0.5f, 0.5f, 0.5f, 1.f}, "color", colorDefaultProperties}};
  bool _enableTesselation{false};
  bool _enableLight{true};

  Uniform<glm::mat4, float> Model() const;
};
}  // namespace Essentials
