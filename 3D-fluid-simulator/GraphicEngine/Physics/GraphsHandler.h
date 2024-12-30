#include <vector>

#include "Entity.h"
#include "Essentials.h"
#include "Uniform.h"
#include "glm/vec3.hpp"

/**
 * @brief Class responsible for serializing data and schedule drawing graphs
 *
 */
struct GraphsHandler
{
  using HeatmapData_t = std::vector<uint8_t>;
  using HeatmapData = Essentials::ParticleBufferProperties;
  float granularity{0.1f};
  std::string value_unit{};
  Essentials::ColorProperty color_type{Essentials::ColorProperty::NONE};

  /**
   * @brief Retrieve particle position based on a defined granularity
   *
   * @param data particle properties
   * @return glm::ivec3
   */
  glm::ivec3 GetDataPosition(HeatmapData const& data);
  /**
   * @brief Convert all available particle properties into a single vector and
   * transform positions of those particles to the world coordinates
   *
   * @param entities Available entities in the simulation
   * @return std::vector<HeatmapData>
   */
  std::vector<HeatmapData> GetGraphData(
      Essentials::EntityContainer const& entities);
  /**
   * @brief Create a serialized data needed to draw graphs in three planes (XY,
   * XZ, YZ)
   *
   * @param data list of particle properties to serialize
   * @return HeatmapData_t const
   */
  HeatmapData_t const SerializeData(std::vector<HeatmapData> const& data);
  /**
   * @brief Schedule graphs generation for a specified serialized data
   *
   * @param data serialized data used for drawing graphs
   */
  void GenerateGraphs(HeatmapData_t const data);

 private:
  /**
   * @brief Add color value based on a chosen property to track as a color value
   * of a particle
   *
   * @param heatmap_data Serialized data to be appended
   * @param data Particle properties used for serialization
   */
  void InsertColorValue(GraphsHandler::HeatmapData_t& heatmap_data,
                        HeatmapData const& data);
};