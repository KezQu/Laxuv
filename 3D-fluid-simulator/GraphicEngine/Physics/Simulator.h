#pragma once

#include "Entity.h"
#include "Essentials.h"
#include "ShaderStorageBuffer.h"
#include "Uniform.h"

class Simulator
{
 public:
  using EntityContainer = std::unordered_map<uint64_t, std::unique_ptr<Entity>>;

 private:
  EntityContainer _entitiesContainer;
  ShaderStorageBuffer<Essentials::TerrainBufferProperties> _terrain;
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
      static_cast<uint32_t>(Essentials::WorldType::SHPERE_WORLD), "worldType"};
  bool _static_timestep{true};

 private:
  Simulator();

 public:
  static Simulator& GetInstance();
  EntityContainer& GetEntities();
  Essentials::SimulationState GetSimulationState();
  void UpdateDeltaTime(float dt);
  Uniform<float>& GetDeltaTime();
  bool IsStaticDtUsed();
  void ToggleTimesetType();
  details::detail_controls_t GetDetails();
  void BindUniforms(uint32_t program_id);
  void BindTerrain(uint32_t program_id);
  void SetSimulationState(Essentials::SimulationState new_global_state);
  void AddObstacle();
  void RemoveObstacle();
  template <typename T>
  void Append(T&& entity);
  void Delete(EntityContainer::key_type id = 0);
  void CleanUp();
};

template <typename T>
void Simulator::Append(T&& entity)
{
  EntityContainer::key_type entity_id = entity.ID();
  _entitiesContainer[entity_id] = std::make_unique<T>(std::move(entity));
}
