#include "Simulator.h"

#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>
#include <ostream>
#include <thread>
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
  // Create an instance at the first call
  static std::unique_ptr<Simulator> instance{nullptr};
  if (instance == nullptr)
  {
    instance = std::unique_ptr<Simulator>(new Simulator{});
  }
  return instance;
}

void Simulator::CleanUp()
{
  // Explicit clear of the simulator instance to prevent destroying instance
  // when OpenGL context is not available
  GetInstance().reset(nullptr);
}

Essentials::EntityContainer& Simulator::GetEntities()
{
  return _entitiesContainer;
}

Essentials::SimulationState Simulator::GetSimulationState()
{
  // Retrieve raw simulation state from the uniform wrapper
  return static_cast<Essentials::SimulationState>(
      _global_simulation_state.GetValue());
}

void Simulator::UpdateDeltaTime(float dt)
{
  // Set value of the time step uniform based on the used method
  if (_static_timestep)
  {
    _global_delta_time = dt;
  }
  else
  {
    _global_delta_time = 1000.f / ImGui::GetIO().Framerate;
  }
}

void Simulator::CreateGraphs()
{
  // Retrieve properties of the entities registered in the simulator
  auto const simulation_data = _graphs.GetGraphData(_entitiesContainer);
  // Launch graph creation in a separate thread to unblock main event loop
  std::thread{[simulation_data, this]()
              {
                auto const heatmap_data =
                    _graphs.SerializeData(simulation_data);
                _graphs.GenerateGraphs(heatmap_data);
              }}
      .detach();
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
  // Changed internal flag indicating method of retrieving time step
  _static_timestep = !_static_timestep;
}

Essentials::DetailControls Simulator::GetDetails()
{
  // Expose properties specific to the world space and simulator
  Essentials::DetailControls details;
  details.push_back({"Space bounds", this->_space_boundaries.ExposeToUI()});
  details.push_back({"Bounds viscosity", this->_bounds_viscosity.ExposeToUI()});
  details.push_back({"World shape", [this]()
                     {
                       ImGui::Combo(
                           "##World_shape",
                           (int32_t*)&this->_global_world_type.GetValue(),
                           Essentials::WorldTypeTolist());
                     }});
  details.push_back({"Ambient light color",
                     this->_simulation_light.ambient_color.ExposeToUI()});
  details.push_back({"Diffuse light color",
                     this->_simulation_light.diffuse.color.ExposeToUI()});
  details.push_back({"Diffuse light direction",
                     this->_simulation_light.diffuse.direction.ExposeToUI()});
  return details;
}

void Simulator::BindUniforms(uint32_t program_id)
{
  // Map global uniform specific to the whole simulation
  _global_delta_time.MapUniform(program_id);
  _global_simulation_state.MapUniform(program_id);
  _global_world_type.MapUniform(program_id);

  _obstacles_number.MapUniform(program_id);

  _space_boundaries.MapUniform(program_id);
  _bounds_viscosity.MapUniform(program_id);

  _simulation_light.ambient_color.MapUniform(program_id);
  _simulation_light.diffuse.color.MapUniform(program_id);
  _simulation_light.diffuse.direction.MapUniform(program_id);
}

void Simulator::BindTerrain(uint32_t program_id)
{
  // Bind terrain buffer to a given program to access terrain data during
  // calculating simulation frame
  _terrain.Bind(program_id);
}

void Simulator::SetSimulationState(Essentials::SimulationState new_global_state)
{
  _global_simulation_state = static_cast<uint32_t>(new_global_state);
}

uint32_t Simulator::AddObstacle(
    Essentials::TerrainBufferProperties const& terrain_properties)
{
  // Add new terrain properties to the terrain buffer
  _terrain.AddToBufferMemory(terrain_properties);
  _obstacles_number = _terrain.Size();

  return _terrain.Size() - 1;
}

void Simulator::RemoveObstacle(Essentials::EntityContainer::key_type id)
{
  // Remove terrain for buffer if provided id points to valid id
  if (id != std::numeric_limits<uint64_t>::max())
  {
    _terrain.RemoveFromBufferMemory(id);
    ReindexObstacles(id);
    _obstacles_number = _terrain.Size();
  }
}
void Simulator::ReindexObstacles(
    Essentials::EntityContainer::key_type removed_id)
{
  // Iterate through all entities to find the ones that are registered as
  // terrain
  for (auto& [uniq_id, entity] : _entitiesContainer)
  {
    // Check if the entity is registered as terrain and if its index appear
    // after the removed one
    if ((entity->GetTerrainId() > removed_id) &&
        (entity->GetTerrainId() != std::numeric_limits<uint64_t>::max()))
    {
      // Decrease the terrain id to keep the correct order of the terrain
      entity->SetTerrainId(entity->GetTerrainId() - 1);
    }
  }
}

void Simulator::CreateEntity(Essentials::EntityType entity_type,
                             Essentials::EntityShape entity_shape)
{
  // Save current simulation state to be able to get back to it
  auto old_sim_state_saved = _global_simulation_state;
  // Set simulation to init state to enable adding new entities
  Simulator::GetInstance()->SetSimulationState(
      Essentials::SimulationState::INIT);
  Essentials::EntityContainer::key_type entity_id{};
  // Create specific entity based on a provided type and shape
  switch (entity_shape)
  {
    case Essentials::EntityShape::CUBE:
      entity_id =
          entity_type == Essentials::EntityType::OBJECT
              ? Append(Object{new Cube{}})
              : Append(Particles{new Cube{Vertex{}, 1.f}, glm::ivec3{5}});
      break;
    case Essentials::EntityShape::SPHERE:
      entity_id =
          entity_type == Essentials::EntityType::OBJECT
              ? Append(Object{new Sphere{}})
              : Append(Particles{new Sphere{Vertex{}, 1.f}, glm::ivec3{5}});
      break;
    case Essentials::EntityShape::LINE:
      entity_id = entity_type == Essentials::EntityType::OBJECT
                      ? Append(Object{new Line{}})
                      : Append(Particles{new Line{}, glm::ivec3{5}});
      break;
    default:
      break;
  }
  // Add objects to the terrain buffer
  if (entity_type == Essentials::EntityType::OBJECT)
  {
    Essentials::TerrainBufferProperties initial_properties{};
    initial_properties.center.w = static_cast<float>(entity_shape);
    auto const terrain_id{AddObstacle(initial_properties)};
    _entitiesContainer[entity_id]->SetTerrainId(terrain_id);
  }
  _global_simulation_state = old_sim_state_saved;
}

void Simulator::Delete(Essentials::EntityContainer::key_type id)
{
  // Delete entity present at the specified id
  RemoveObstacle(_entitiesContainer[id]->GetTerrainId());
  if (id != 0)
  {
    _entitiesContainer.erase(id);
  }
  else
  {
    LOG << "No entity id specified for deletion" << std::endl;
  }
}
