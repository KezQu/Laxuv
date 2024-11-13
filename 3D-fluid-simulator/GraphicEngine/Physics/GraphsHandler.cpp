#include "GraphsHandler.h"

#include <fstream>
#include <memory>
#include <vector>

glm::ivec3 GraphsHandler::GetDataPosition(
    Essentials::ParticleBufferProperties const& data, uint64_t offset)
{
  glm::ivec3 rescaled_position{};
  rescaled_position = glm::round((data.position + offset / 2.f) / granularity);
  return rescaled_position;
}

std::vector<Essentials::ParticleBufferProperties> GraphsHandler::GetGraphData(
    Essentials::EntityContainer const& entities)
{
  std::vector<Essentials::ParticleBufferProperties> data;
  for (auto& [id, entity] : entities)
  {
    auto const& physics_buffer = entity->GetPhysicsBuffer();
    auto const buffer_data =
        physics_buffer.GetBufferSubData(0U, physics_buffer.Size());
    data.insert(data.end(), buffer_data.begin(), buffer_data.end());
  }
  return data;
}
GraphsHandler::HeatmapData GraphsHandler::SerializeData(
    std::vector<Essentials::ParticleBufferProperties> const& data,
    uint64_t space_bounds)
{
  uint64_t const resolution = static_cast<uint64_t>(space_bounds / granularity);
  uint64_t const heatmap_size =
      static_cast<uint64_t>(std::pow(resolution, 2) * 4);

  uint8_t* const heatmap_data = new uint8_t[heatmap_size * 3];
  std::memset(heatmap_data, 0, heatmap_size * 3);

  auto draw_data = [=, this](Projection proj_type, glm::ivec2 position,
                             glm::vec4 data_color, float data_radius)
  {
    uint16_t scaled_radius = glm::round(data_radius / granularity);
    for (int col_offset = 0; col_offset < 4; col_offset++)
    {
      uint64_t offset = proj_type * heatmap_size + 4 * (position.x) +
                        resolution * 4 * (position.y) + col_offset;
      auto color_value = static_cast<uint8_t>(data_color[col_offset] * 0xFF);
      for (int i = -scaled_radius / 2; i < scaled_radius / 2; i++)
      {
        for (int j = -scaled_radius / 2; j < scaled_radius / 2; j++)
        {
          int64_t radius_offset = j * resolution * 4 + i * 4;
          if ((offset + radius_offset) > heatmap_size * proj_type &&
              (offset + radius_offset) < heatmap_size * (proj_type + 1))
          {
            heatmap_data[offset + radius_offset] = color_value;
          }
        }
      }
    }
  };

  for (int i = 0; i < data.size(); i++)
  {
    auto const position = GetDataPosition(data[i], space_bounds);
    draw_data(Projection::XY, {position.x, position.y}, data[i].color,
              data[i].position.w);
    draw_data(Projection::YZ, {position.z, position.y}, data[i].color,
              data[i].position.w);
    draw_data(Projection::XZ, {position.x, position.z}, data[i].color,
              data[i].position.w);
  }
  return heatmap_data;
}

void GraphsHandler::GenerateGraphs(GraphsHandler::HeatmapData data,
                                   uint64_t space_bounds)
{
  uint64_t const resolution = static_cast<uint64_t>(space_bounds / granularity);

  std::filesystem::path abs_path{__FILE__};
  std::string const script_path{abs_path.parent_path().string() + "/plot.py"};
  std::string const data_filepath{abs_path.parent_path().string() + "/tmp.dat"};

  std::ofstream file{data_filepath, std::ios_base::binary};
  file.write(reinterpret_cast<char const* const>(data),
             resolution * resolution * 4 * 3);
  file.close();
  delete[] data;

  std::cout << "python3 " + script_path + " --filename " + data_filepath +
                   " --resolution " + std::to_string(resolution)
            << std::endl;

  std::system(("python3 " + script_path + " --filename " + data_filepath +
               " --resolution " + std::to_string(resolution))
                  .c_str());
}
