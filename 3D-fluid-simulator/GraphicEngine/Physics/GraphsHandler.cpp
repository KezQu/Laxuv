#include "GraphsHandler.h"

#include <fstream>
#include <memory>
#include <vector>

#include "Essentials.h"
#include "glm/geometric.hpp"

glm::ivec3 GraphsHandler::GetDataPosition(
    Essentials::ParticleBufferProperties const& data, uint64_t offset)
{
  glm::ivec3 rescaled_position{};
  rescaled_position = glm::round((data.position + offset / 2.f) / granularity);
  return rescaled_position;
}

std::vector<GraphsHandler::HeatmapData> GraphsHandler::GetGraphData(
    Essentials::EntityContainer const& entities)
{
  std::vector<GraphsHandler::HeatmapData> data;
  for (auto& [id, entity] : entities)
  {
    auto const& physics_buffer = entity->GetPhysicsBuffer();
    if (entity->GetColorType() != Essentials::ColorProperty::NONE)
    {
      color_type = entity->GetColorType();
      auto const buffer_data =
          physics_buffer.GetBufferSubData(0U, physics_buffer.Size());
      data.insert(data.end(), buffer_data.begin(), buffer_data.end());
    }
  }
  return data;
}
GraphsHandler::HeatmapData_t const GraphsHandler::SerializeData(
    std::vector<GraphsHandler::HeatmapData> const& data, uint64_t space_bounds)
{
  GraphsHandler::HeatmapData_t heatmap_data;

  auto insert_data = [&heatmap_data, this](
                         Projection proj_type, glm::ivec2 position,
                         Essentials::ParticleBufferProperties const& properties,
                         float data_radius)
  {
    for (int i = -static_cast<int>(data_radius); i < data_radius; i++)
    {
      for (int j = -static_cast<int>(data_radius); j < data_radius; j++)
      {
        glm::ivec2 pos_with_offset = position + glm::ivec2{i, j};
        std::span<uint8_t> pos_x{reinterpret_cast<uint8_t*>(&pos_with_offset.x),
                                 4};
        std::span<uint8_t> pos_y{reinterpret_cast<uint8_t*>(&pos_with_offset.y),
                                 4};
        heatmap_data.insert(heatmap_data.end(), pos_x.begin(), pos_x.end());
        heatmap_data.insert(heatmap_data.end(), pos_y.begin(), pos_y.end());
        InsertColorValue(heatmap_data, properties);
      }
    }
  };

  std::for_each(
      data.begin(), data.end(),
      [&heatmap_data, this, &space_bounds, &insert_data](auto const& data)
      {
        auto const position = GetDataPosition(data, space_bounds);
        insert_data(Projection::XY, {position.y, position.x}, data,
                    data.position.w / granularity);
      });
  std::for_each(
      data.begin(), data.end(),
      [&heatmap_data, this, &space_bounds, &insert_data](auto const& data)
      {
        auto const position = GetDataPosition(data, space_bounds);
        insert_data(Projection::YZ, {position.y, position.z}, data,
                    data.position.w / granularity);
      });
  std::for_each(
      data.begin(), data.end(),
      [&heatmap_data, this, &space_bounds, &insert_data](auto const& data)
      {
        auto const position = GetDataPosition(data, space_bounds);
        insert_data(Projection::XZ, {position.x, position.z}, data,
                    data.position.w / granularity);
      });
  return heatmap_data;
}

void GraphsHandler::GenerateGraphs(GraphsHandler::HeatmapData_t const data,
                                   uint64_t space_bounds)
{
  std::filesystem::path abs_path{__FILE__};
  std::string const script_path{abs_path.parent_path().string() + "/plot.py"};
  std::string const data_filepath{abs_path.parent_path().string() + "/tmp.dat"};

  std::ofstream file{data_filepath, std::ios_base::binary};
  file.write(reinterpret_cast<char const* const>(data.data()), data.size());
  file.close();

  std::cout << "python3 " + script_path + " --filename " + data_filepath +
                   " --granularity " + std::to_string(granularity)
            << std::endl;

  std::system(("python3 " + script_path + " --filename " + data_filepath +
               " --granularity " + std::to_string(granularity))
                  .c_str());
}
void GraphsHandler::InsertColorValue(
    GraphsHandler::HeatmapData_t& heatmap_data,
    Essentials::ParticleBufferProperties const& properties)
{
  float value_to_draw = 0.f;
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
      value_to_draw = properties.MassDensityPressureDro_Dt.z;
      break;
  }
  std::span<uint8_t> value_in_byte{reinterpret_cast<uint8_t*>(&value_to_draw),
                                   4};
  heatmap_data.insert(heatmap_data.end(), value_in_byte.begin(),
                      value_in_byte.end());
}