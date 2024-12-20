#include <vector>

#include "Entity.h"
#include "Essentials.h"
#include "Uniform.h"
#include "glm/vec3.hpp"

struct GraphsHandler
{
  float granularity{0.1f};
  Essentials::ColorProperty color_type{Essentials::ColorProperty::NONE};
  using HeatmapData_t = std::vector<uint8_t>;
  using HeatmapData = Essentials::ParticleBufferProperties;

  glm::ivec3 GetDataPosition(Essentials::ParticleBufferProperties const& data,
                             uint64_t space_bounds);
  std::vector<HeatmapData> GetGraphData(
      Essentials::EntityContainer const& entities);
  HeatmapData_t const SerializeData(std::vector<HeatmapData> const& data,
                                    uint64_t space_bounds);
  void GenerateGraphs(HeatmapData_t const data, uint64_t space_bounds);

 private:
  void DrawData(HeatmapData_t data, uint64_t offset, glm::ivec2 position);
  void InsertColorValue(GraphsHandler::HeatmapData_t& heatmap_data,
                        Essentials::ParticleBufferProperties const& data);
};