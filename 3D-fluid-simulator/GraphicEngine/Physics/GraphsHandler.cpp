#include "GraphsHandler.h"

#include <fstream>
#include <memory>
#include <span>
#include <vector>

#include "Essentials.h"
#include "glm/geometric.hpp"

glm::ivec3 GraphsHandler::GetDataPosition(
    Essentials::ParticleBufferProperties const& data)
{
  glm::ivec3 rescaled_position{};
  // Transform floating point position to an integer with a given precision
  rescaled_position = glm::round((data.position) / granularity);
  return rescaled_position;
}

std::vector<GraphsHandler::HeatmapData> GraphsHandler::GetGraphData(
    Essentials::EntityContainer const& entities)
{
  std::vector<GraphsHandler::HeatmapData> data;
  for (auto& [id, entity] : entities)
  {
    // Retrieve entity buffer containing needed data for serialization
    auto const& physics_buffer = entity->GetPhysicsBuffer();
    // Include only dynamic entities to be drawn
    if (entity->GetPhysicsType() == Essentials::PhysicsType::DYNAMIC)
    {
      // Get shape properties to handle entity shape radius and orientation in
      // world space
      auto const& entity_shape_properties = entity->GetShapeProperties();
      color_type = entity->GetColorType();
      // Retrieve properties of an entity from GPU
      auto buffer_data =
          physics_buffer.GetBufferSubData(0U, physics_buffer.Size());
      // Transform every object and particle to the world space
      std::for_each(buffer_data.begin(), buffer_data.end(),
                    [&entity_shape_properties](auto& entity_data) {
                      entity_data.position =
                          entity_shape_properties.Model() *
                          entity_data.position *
                          entity_shape_properties._radius.GetValue();
                    });
      data.insert(data.end(), buffer_data.begin(), buffer_data.end());
    }
  }
  return data;
}
GraphsHandler::HeatmapData_t const GraphsHandler::SerializeData(
    std::vector<GraphsHandler::HeatmapData> const& data)
{
  GraphsHandler::HeatmapData_t heatmap_data;

  // Helper lambda providing schema for serialization
  auto insert_data = [&heatmap_data, this](
                         glm::ivec3 position,
                         Essentials::ParticleBufferProperties const& properties,
                         float data_radius) {
    // Convert position into binary format
    std::span<uint8_t> pos_x{reinterpret_cast<uint8_t*>(&position.x), 4};
    std::span<uint8_t> pos_y{reinterpret_cast<uint8_t*>(&position.y), 4};
    std::span<uint8_t> pos_z{reinterpret_cast<uint8_t*>(&position.z), 4};
    // Insert serialized position and color
    heatmap_data.insert(heatmap_data.end(), pos_x.begin(), pos_x.end());
    heatmap_data.insert(heatmap_data.end(), pos_y.begin(), pos_y.end());
    heatmap_data.insert(heatmap_data.end(), pos_z.begin(), pos_z.end());
    InsertColorValue(heatmap_data, properties);
  };
  // Serialize each entity marked for serialization using lambda
  std::for_each(data.begin(), data.end(),
                [&heatmap_data, this, &insert_data](auto const& data) {
                  auto const position = GetDataPosition(data);
                  insert_data(position, data, data.position.w / granularity);
                });
  return heatmap_data;
}

void GraphsHandler::GenerateGraphs(GraphsHandler::HeatmapData_t const data)
{
  // Recreate absolute path to graph generator script and to data file
  std::filesystem::path abs_path{__FILE__};
  std::string const script_path{abs_path.parent_path().string() + "/plot.py"};
  std::string const data_filepath{abs_path.parent_path().string() + "/tmp.dat"};

  // Save serialized data to a temporary data file in binary format
  std::ofstream file{data_filepath, std::ios_base::binary};
  file.write(reinterpret_cast<char const* const>(data.data()), data.size());
  file.close();

  // Run graph drawing script
  std::system(("python3 " + script_path + " --filename " + data_filepath +
               " --granularity " + std::to_string(granularity))
                  .c_str());
}
void GraphsHandler::InsertColorValue(
    GraphsHandler::HeatmapData_t& heatmap_data,
    Essentials::ParticleBufferProperties const& properties)
{
  float value_to_draw = 0.f;
  // Select property to be represented by a color based on the user input
  switch (color_type)
  {
    case Essentials::ColorProperty::NONE:
      break;
    case Essentials::ColorProperty::CUSTOM:
      break;
    case Essentials::ColorProperty::VELOCITY:
      value_to_draw = glm::length(glm::vec3{properties.velocityDFSPHfactor.x,
                                            properties.velocityDFSPHfactor.y,
                                            properties.velocityDFSPHfactor.z});
      break;
    case Essentials::ColorProperty::DENSITY_ERROR:
      value_to_draw = properties.MassDensityPressureDro_Dt.y;
      break;
    case Essentials::ColorProperty::DIVERGENCE_ERROR:
      value_to_draw = properties.MassDensityPressureDro_Dt.w;
      break;
    case Essentials::ColorProperty::PRESSURE:
      value_to_draw = glm::abs(properties.MassDensityPressureDro_Dt.z);
      break;
  }
  // Serialize given color and save to the serialized data
  std::span<uint8_t> value_in_byte{reinterpret_cast<uint8_t*>(&value_to_draw),
                                   4};
  heatmap_data.insert(heatmap_data.end(), value_in_byte.begin(),
                      value_in_byte.end());
}