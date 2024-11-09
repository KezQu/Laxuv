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
uint8_t const* const GraphsHandler::SerializeData(
    std::vector<Essentials::ParticleBufferProperties> const& data,
    uint64_t space_bounds)
{
  uint64_t const resolution = static_cast<uint64_t>(space_bounds / granularity);
  uint64_t const heatmap_size =
      static_cast<uint64_t>(std::pow(resolution, 2) * 4);

  uint8_t* const heatmap_data = new uint8_t[heatmap_size * 3];
  std::memset(heatmap_data, 0, heatmap_size * 3);
  for (int i = 0; i < data.size(); i++)
  {
    auto const position = GetDataPosition(data[i], space_bounds);
    for (int j = 0; j < 4; j++)
    {
      {
        uint64_t offset = Projection::XY * heatmap_size + 4 * (position.x) +
                          resolution * 4 * (position.y) + j;
        auto color_value = static_cast<uint8_t>(data[i].color[j] * 0xFF);
        heatmap_data[offset] = color_value;
      }
      {
        uint64_t offset = Projection::YZ * heatmap_size + 4 * (position.z) +
                          resolution * 4 * (position.y) + j;
        auto color_value = static_cast<uint8_t>(data[i].color[j] * 0xFF);
        heatmap_data[offset] = color_value;
      }
      {
        uint64_t offset = Projection::XZ * heatmap_size + 4 * (position.x) +
                          resolution * 4 * (position.z) + j;
        auto color_value = static_cast<uint8_t>(data[i].color[j] * 0xFF);
        heatmap_data[offset] = color_value;
      }
    }
  }
  return heatmap_data;
}

void GraphsHandler::GenerateGraphs(uint8_t const* const data,
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

  std::system(("python3 " + script_path + " --filename " + data_filepath +
               " --resolution " + std::to_string(resolution))
                  .c_str());
}
