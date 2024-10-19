#pragma once

#include <Entity.h>
#include <Essentials.h>
#include <Uniform.h>

class Simulator
{
 public:
  using EntityContainer = std::unordered_map<uint64_t, std::unique_ptr<Entity>>;

 private:
  EntityContainer _entitiesContainer;
  Uniform<float> _space_boundries{100.f, "spaceLimiter"};
  Uniform<float> _bounds_viscosity{0.95f, "boundsViscosity"};
  Uniform<float> _global_delta_time{0.f, "dt", 1e-3f};
  Essentials::SimulationState _globalSimulationState;

 private:
  Simulator() = default;

 public:
  static Simulator& GetInstance();
  EntityContainer& GetEntities();
  Essentials::SimulationState GetSimulationState();
  void UpdateDeltaTime();
  void UpdateDeltaTime(float dt);
  Uniform<float>& GetDeltaTime();
  details::details_map GetDetails();
  void BindUniforms(uint32_t program_id);
  void SetSimulationState(Essentials::SimulationState newGlobalState);
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