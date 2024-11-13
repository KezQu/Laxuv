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
  float granularity{0.1f};
  using HeatmapData = uint8_t const* const;

  glm::ivec3 GetDataPosition(Essentials::ParticleBufferProperties const& data,
                             uint64_t space_bounds);
  std::vector<Essentials::ParticleBufferProperties> GetGraphData(
      Essentials::EntityContainer const& entities);
  HeatmapData SerializeData(
      std::vector<Essentials::ParticleBufferProperties> const& data,
      uint64_t space_bounds);
  void GenerateGraphs(HeatmapData data, uint64_t space_bounds);

 private:
  void DrawData(HeatmapData data, uint64_t offset, glm::ivec2 position);
};