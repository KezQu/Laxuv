#include "Simulator.h"

#include <cstdint>
#include <cstring>
#include <fstream>
#include <limits>
#include <memory>
#include <ostream>
#include <unordered_map>
#include <vector>

#include "Cube.h"
#include "Entity.h"
#include "Essentials.h"
#include "Line.h"
#include "Object.h"
#include "Particles.h"
#include "Point.h"
#include "Sphere.h"
#include "Square.h"
#include "Uniform.h"
#include "VertexArray.h"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"

Simulator::Simulator() : _terrain{"terrainBuffer"} {}

std::unique_ptr<Simulator>& Simulator::GetInstance()
{
  static std::unique_ptr<Simulator> instance{nullptr};
  if (instance == nullptr)
  {
    instance = std::unique_ptr<Simulator>(new Simulator{});
  }
  return instance;
}

void Simulator::CleanUp()
{
  GetInstance().reset(nullptr);
}

Simulator::EntityContainer& Simulator::GetEntities()
{
  return _entitiesContainer;
}

Essentials::SimulationState Simulator::GetSimulationState()
{
  return static_cast<Essentials::SimulationState>(
      _global_simulation_state.GetValue());
}

void Simulator::UpdateDeltaTime(float dt)
{
  if (_static_timestep)
  {
    _global_delta_time = dt;
  }
  else
  {
    _global_delta_time = 1000.f / ImGui::GetIO().Framerate;
  }
}

template <uint64_t N>
struct HeatmapData
{
  using heatType = uint8_t[N][N][4];
  heatType map_data;
  uint64_t resolution = N;
};

void Simulator::CreateGraphs()
{
  static HeatmapData<2400> heatmap;
  auto const res = heatmap.resolution;
  std::vector<Essentials::ParticleBufferProperties> data;
  for (auto& [id, entity] : _entitiesContainer)
  {
    auto const& physics_buffer = entity->GetPhysicsBuffer();
    auto const buffer_data =
        physics_buffer.GetBufferSubData(0U, physics_buffer.Size());
    data.insert(data.end(), buffer_data.begin(), buffer_data.end());
  }

  std::memset(heatmap.map_data, 0, sizeof(heatmap.map_data));
  for (int i = 0; i < data.size(); i++)
  {
    for (int j = 0; j < 4; j++)
    {
      heatmap.map_data[(int)data[i].position.x][(int)data[i].position.y][j] =
          static_cast<uint8_t>(data[i].color[j] * 0xFF);
    }
  }
  std::filesystem::path abs_path(__FILE__);
  std::string system_path(abs_path.parent_path().string());

  std::ofstream file{system_path + "/tmp.dat", std::ios_base::binary};
  std::cout << sizeof(heatmap.map_data) << std::endl;
  std::cout << "python3 " + system_path + "/plot.py --filename " + system_path +
                   "/tmp.dat"
            << std::endl;
  file.write(reinterpret_cast<char*>(heatmap.map_data),
             sizeof(heatmap.map_data));

  std::system(("python3 " + system_path + "/plot.py --filename " + system_path +
               "/tmp.dat")
                  .c_str());
}

Uniform<float>& Simulator::GetDeltaTime()
{
  return _global_delta_time;
}

bool Simulator::IsStaticDtUsed()
{
  return _static_timestep;
}

void Simulator::ToggleTimesetType()
{
  _static_timestep = !_static_timestep;
}

details::detail_controls_t Simulator::GetDetails()
{
  details::detail_controls_t details;
  details.push_back({"Space bounds", this->_space_boundries.ExposeToUI()});
  details.push_back({"Bounds viscosity", this->_bounds_viscosity.ExposeToUI()});
  details.push_back({"World shape", [this]()
                     {
                       ImGui::Combo(
                           "##World_shape",
                           (int32_t*)&this->_global_world_type.GetValue(),
                           Essentials::WorldTypeTolist());
                     }});
  details.push_back({"Ambient light color",
                     this->_simulaton_light.ambient_color.ExposeToUI()});
  details.push_back({"Diffuse light color",
                     this->_simulaton_light.diffuse.color.ExposeToUI()});
  details.push_back({"Diffuse light direction",
                     this->_simulaton_light.diffuse.direction.ExposeToUI()});
  return details;
}

void Simulator::BindUniforms(uint32_t program_id)
{
  _global_delta_time.MapUniform(program_id);
  _global_simulation_state.MapUniform(program_id);
  _global_world_type.MapUniform(program_id);

  _obstacles_number.MapUniform(program_id);

  _space_boundries.MapUniform(program_id);
  _bounds_viscosity.MapUniform(program_id);

  _simulaton_light.ambient_color.MapUniform(program_id);
  _simulaton_light.diffuse.color.MapUniform(program_id);
  _simulaton_light.diffuse.direction.MapUniform(program_id);
}

void Simulator::BindTerrain(uint32_t program_id)
{
  _terrain.Bind(program_id);
}

void Simulator::SetSimulationState(Essentials::SimulationState new_global_state)
{
  _global_simulation_state = static_cast<uint32_t>(new_global_state);
}

uint32_t Simulator::AddObstacle(
    Essentials::TerrainBufferProperties const& terrain_properties)
{
  _terrain.AddToBufferMemory(terrain_properties);
  _obstacles_number = _terrain.Size();

  return _terrain.Size() - 1;
}

void Simulator::RemoveObstacle(EntityContainer::key_type id)
{
  if (id != std::numeric_limits<uint64_t>::max())
  {
    _terrain.RemoveFromBufferMemory(id);
    _obstacles_number = _terrain.Size();
  }
}

void Simulator::CreateEntity(Essentials::EntityType entity_type,
                             Essentials::EntityShape entity_shape)
{
  auto old_sim_state_saved = _global_simulation_state;
  Simulator::GetInstance()->SetSimulationState(
      Essentials::SimulationState::INIT);
  EntityContainer::key_type entity_id{};
  switch (entity_shape)
  {
    case Essentials::EntityShape::POINT:
      entity_id = entity_type == Essentials::EntityType::OBJECT
                      ? Append(Object{new Point{}})
                      : Append(Particles{new Point{}, glm::uvec3{5U}});
      break;
    case Essentials::EntityShape::LINE:
      entity_id = entity_type == Essentials::EntityType::OBJECT
                      ? Append(Object{new Line{}})
                      : Append(Particles{new Line{}, glm::uvec3{5U}});
      break;
    case Essentials::EntityShape::SQUARE:
      entity_id =
          entity_type == Essentials::EntityType::OBJECT
              ? Append(Object{new Square{}})
              : Append(Particles{new Square{Vertex{}, 1.f}, glm::uvec3{5U}});
      break;
    case Essentials::EntityShape::CUBE:
      entity_id =
          entity_type == Essentials::EntityType::OBJECT
              ? Append(Object{new Cube{}})
              : Append(Particles{new Cube{Vertex{}, 1.f}, glm::uvec3{5U}});
      break;
    case Essentials::EntityShape::SPHERE:
      entity_id =
          entity_type == Essentials::EntityType::OBJECT
              ? Append(Object{new Sphere{}})
              : Append(Particles{new Sphere{Vertex{}, 1.f}, glm::uvec3{5U}});
      break;
    default:
      break;
  }
  if (entity_type == Essentials::EntityType::OBJECT)
  {
    Essentials::TerrainBufferProperties initial_properties{};
    initial_properties.center.w = static_cast<float>(entity_shape);
    auto const terrain_id{AddObstacle(initial_properties)};
    _entitiesContainer[entity_id]->SetTerrainId(terrain_id);
  }
  _global_simulation_state = old_sim_state_saved;
}

void Simulator::Delete(EntityContainer::key_type id)
{
  RemoveObstacle(_entitiesContainer[id]->GetTerrainId());
  if (id != 0)
  {
    _entitiesContainer.erase(id);
  }
  else
  {
    _entitiesContainer.erase(std::prev(_entitiesContainer.end()));
  }
}
