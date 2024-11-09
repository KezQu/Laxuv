#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>

#include "CPUBuffer.h"
#include "Entity.h"
#include "Essentials.h"
#include "GraphsHandler.h"
#include "ShaderStorageBuffer.h"
#include "Uniform.h"

class Simulator
{
 private:
  GraphsHandler _graphs;
  Essentials::EntityContainer _entitiesContainer;
  ShaderStorageBuffer<Essentials::TerrainBufferProperties, CPUBuffer> _terrain;
  Essentials::Light _simulaton_light;
  Uniform<uint32_t> _obstacles_number{0, "MaxObstacles"};
  Uniform<float> _space_boundries{100.f, "spaceLimiter",
                                  ValueProperties{1.f, 1200.f, 1.f, "%.1f mm"}};
  Uniform<float> _bounds_viscosity{0.95f, "boundsViscosity",
                                   ValueProperties{0.f, 1.f, 1.f, "%.2f"}};
  Uniform<float> _global_delta_time{
      6.f, "dt", ValueProperties{1.f, 1000.f, 1e-3f, "%1.f ms"}};
  Uniform<uint32_t> _global_simulation_state{
      static_cast<uint32_t>(Essentials::SimulationState::INIT),
      "SimulatorState"};
  Uniform<uint32_t> _global_world_type{
      static_cast<uint32_t>(Essentials::WorldType::SHPERE_T), "worldType"};
  bool _static_timestep{true};

 private:
  Simulator();
  template <typename T>
  Essentials::EntityContainer::key_type Append(T&& entity);
  uint32_t AddObstacle(
      Essentials::TerrainBufferProperties const& terrain_properties);
  void RemoveObstacle(Essentials::EntityContainer::key_type id);

 public:
  static std::unique_ptr<Simulator>& GetInstance();
  static void CleanUp();
  Essentials::EntityContainer& GetEntities();
  Essentials::SimulationState GetSimulationState();
  void UpdateDeltaTime(float dt);
  void CreateGraphs();
  Uniform<float>& GetDeltaTime();
  bool IsStaticDtUsed();
  void ToggleTimesetType();
  Essentials::DetailControls GetDetails();
  void BindUniforms(uint32_t program_id);
  void BindTerrain(uint32_t program_id);
  void SetSimulationState(Essentials::SimulationState new_global_state);
  void CreateEntity(Essentials::EntityType entity_type,
                    Essentials::EntityShape entity_shape);
  void Delete(Essentials::EntityContainer::key_type id = 0);
};

template <typename T>
Essentials::EntityContainer::key_type Simulator::Append(T&& entity)
{
  Essentials::EntityContainer::key_type entity_id = entity.ID();
  _entitiesContainer[entity_id] = std::make_unique<T>(std::move(entity));
  return entity_id;
}
