#include "Simulator.h"

#include <cstdint>
#include <limits>
#include <memory>
#include <unordered_map>

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
  return details;
}

void Simulator::BindUniforms(uint32_t program_id)
{
  _obstacles_number.MapUniform(program_id);
  _global_delta_time.MapUniform(program_id);
  _space_boundries.MapUniform(program_id);
  _bounds_viscosity.MapUniform(program_id);
  _global_simulation_state.MapUniform(program_id);
  _global_world_type.MapUniform(program_id);
}

void Simulator::BindTerrain(uint32_t program_id)
{
  _terrain.Bind(program_id);
  auto lookupBuffer = _terrain.GetBufferSubData(0U, _terrain.Size());
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
    initial_properties.bounds.w = static_cast<float>(entity_shape);
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
