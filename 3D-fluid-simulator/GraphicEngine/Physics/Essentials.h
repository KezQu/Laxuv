#pragma once

#include <cstdint>
#include <utility>
#include <vector>

#include "Uniform.h"
#include "glm/fwd.hpp"

namespace Essentials
{
constexpr uint32_t MaxNeighbors = 512;
constexpr float length_scale = 1;

/**
 * @brief Helper enum to provide information about entity types to the shaders
 *
 */
enum class EntityType : uint32_t
{
  NONE,
  OBJECT,
  PARTICLES
};
/**
 * @brief Helper function to map entity type enum to list as string with null
 * terminated elements
 *
 * @return char const*
 */
char const* EntityTypeTolist() noexcept;

/**
 * @brief Helper enum to provide information about available entity shapes to
 * the shaders
 *
 */
enum class EntityShape : uint32_t
{
  NONE,
  CUBE,
  SPHERE,
  LINE,
};
/**
 * @brief Helper function to map entity shape enums to list as string with null
 * terminated elements
 *
 * @return char const*
 */
char const* EntityShapeTolist() noexcept;

/**
 * @brief Helper enum to provide information about available distribution shapes
 * for particles to the shaders
 *
 */
enum class DistributionShape : uint32_t
{
  UNDEFINED,
  LINE,
  SQUARE,
  CUBE
};
/**
 * @brief Helper function to map distribution shape enums to list as string with
 * null terminated elements
 *
 * @return char const*
 */
char const* DistShapeTolist() noexcept;

/**
 * @brief Helper enum to provide information about available simulation states
 * to the shaders
 *
 */
enum class SimulationState : uint32_t
{
  NONE,
  IDLE,
  INIT,
  SIMULATION,
  GEN_FRAME,
  GRAPHS
};

/**
 * @brief Helper enum to provide information about available world bounds shape
 * to the shaders
 *
 */
enum class WorldType : uint32_t
{
  NONE,
  CUBE_T,
  SPHERE_T
};
/**
 * @brief Helper function to map world bounds shape enums to list as string with
 * null terminated elements
 *
 * @return char const*
 */
char const* WorldTypeTolist() noexcept;

/**
 * @brief Helper enum to provide information about available physics types
 * to the shaders
 *
 */
enum class PhysicsType : uint32_t
{
  NONE,
  STATIC,
  DYNAMIC
};
/**
 * @brief Helper function to map physics type enums to list as string with null
 * terminated elements
 *
 * @return char const*
 */
char const* PhysTypesTolist() noexcept;

/**
 * @brief Helper enum to provide information about available colors based on the
 * specified particle property to the shaders
 *
 */
enum class ColorProperty : uint32_t
{
  NONE,
  CUSTOM,
  VELOCITY,
  DENSITY_ERROR,
  DIVERGENCE_ERROR,
  PRESSURE,
};
/**
 * @brief Helper function to map available color enums for particles  to list
 * as string with null terminated elements
 *
 * @return char const*
 */
char const* ColorPropertyTolistParticles() noexcept;
/**
 * @brief Helper function to map available color enums for objects to list as
 * string with null terminated elements
 *
 * @return char const*
 */
char const* ColorPropertyTolistObject() noexcept;

/**
 * @brief Helper struct encapsulating properties for terrain entities
 *
 */
struct TerrainBufferProperties
{
  glm::vec4 center{0};
  glm::vec4 bounds{0};
  glm::mat4 model{0};
};

/**
 * @brief Helper struct encapsulating properties of a single particle
 *
 */
struct ParticleBufferProperties
{
  glm::vec4 velocityDFSPHfactor{0};
  glm::vec4 position{0};
  glm::vec4 MassDensityPressureDro_Dt{0};
  glm::vec4 color{0};
  uint32_t neighbors[MaxNeighbors];
};

auto const colorDefaultProperties = ValueProperties{0.f, 1.f, 1.f, "%.2f"};

/**
 * @brief Helper struct encapsulating properties of a global simulation light
 *
 */
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

/**
 * @brief Helper struct encapsulating global properties shared across all
 * particles
 *
 */
struct ParticleProperties
{
  Uniform<glm::vec3, float> init_velocity{
      glm::vec3{0.f}, "initVelocity",
      ValueProperties{-1000.f, 1000.f, 1.f, "%.1f mm/s"}};
  Uniform<float> particle_spacing{
      1.f, "particleSpacing",
      ValueProperties{0.1f, 600.f, length_scale, "%.1f mm"}};
  Uniform<uint32_t> distribution_shape{
      static_cast<uint32_t>(DistributionShape::CUBE), "DistributionShape"};
  Uniform<float> influence_kernel{
      2.5f, "influenceKernel",
      ValueProperties{0.1f, 600.f, length_scale, "%.1f mm"}};
  Uniform<float> kernel_a{3.5f, "kernel_a",
                          ValueProperties{3.f, 100.f, 1.f, "%.1f"}};
  Uniform<float> mass{1000.f, "init_mass",
                      ValueProperties{0.1f, 1e+4f, 1e-9f, "%.1f ug"}};
  Uniform<float> viscosity_factor{
      10.f, "viscosityFactor",
      ValueProperties{0.f, 1000.f, 1e-3f, "%.1f mPa*s"}};
};

/**
 * @brief Helper struct encapsulating properties of a shape
 *
 */
struct ShapeProperties
{
  Uniform<glm::vec3, float> _location{
      glm::vec3{0.f}, "location",
      ValueProperties{-1200.f, 1200.f, length_scale, "%.1f mm"}};
  Uniform<glm::vec3, float> _scale{glm::vec3{1.f}, "scale",
                                   ValueProperties{0.f, 100.f, 1.f, "%.1f"}};
  Uniform<glm::vec3, float> _rotation{
      glm::vec3{0.f}, "rotation",
      ValueProperties{-360.f, 360.f, 1.f, "%.1f deg"}};
  Uniform<glm::vec3, float> _center{
      glm::vec3{0.f}, "center",
      ValueProperties{-1200.f, 1200.f, length_scale, "%.1f mm"}};
  Uniform<uint32_t> _subdivision{5U, "subdivision", ValueProperties{1U, 50U}};
  Uniform<float> _radius{1.f, "shapeRadius",
                         ValueProperties{0.1f, 600.f, length_scale, "%.1f mm"}};
  std::pair<Uniform<uint32_t>, Uniform<glm::vec4, float>> _color{
      {static_cast<uint32_t>(Essentials::ColorProperty::NONE), "colorType"},
      {glm::vec4{0.5f, 0.5f, 0.5f, 1.f}, "color", colorDefaultProperties}};
  bool _enableTesselation{false};
  bool _enableLight{true};

  /**
   * @brief Method providing model matrix for a given shape
   *
   * @return Uniform<glm::mat4, float>
   */
  Uniform<glm::mat4, float> Model() const;
};

}  // namespace Essentials
