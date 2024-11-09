#include <vector>

#include "Entity.h"
#include "Essentials.h"
#include "Uniform.h"
#include "glm/vec3.hpp"

// struct Sections : public Entity
// {
//   Square x{glm::vec3{0.f}, glm::uvec4{0x80, 0x80, 0x80, 0x30}};
//   Square y{glm::vec3{0.f}, glm::uvec4{0x80, 0x80, 0x80, 0x30}};
//   Square z{glm::vec3{0.f}, glm::uvec4{0x80, 0x80, 0x80, 0x30}};
// };

enum Projection : uint32_t
{
  XY,
  YZ,
  XZ
};

struct GraphsHandler
{
  // Sections cross_sections;
  // void* heatmap_data;
  float granularity{1.f};

  glm::ivec3 GetDataPosition(Essentials::ParticleBufferProperties const& data,
                             uint64_t space_bounds);
  std::vector<Essentials::ParticleBufferProperties> GetGraphData(
      Essentials::EntityContainer const& entities);
  uint8_t const* const SerializeData(
      std::vector<Essentials::ParticleBufferProperties> const& data,
      uint64_t space_bounds);
  void GenerateGraphs(uint8_t const* const data, uint64_t space_bounds);
};