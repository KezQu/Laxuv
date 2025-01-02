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

/**
 * @brief Class responsible for simulation. Creating and deleting entities and
 * controlling simulation state
 *
 */
class Simulator
{
 private:
  GraphsHandler _graphs;
  Essentials::EntityContainer _entitiesContainer;
  ShaderStorageBuffer<Essentials::TerrainBufferProperties, CPUBuffer> _terrain;
  Essentials::Light _simulation_light;
  Uniform<uint32_t> _obstacles_number{0, "MaxObstacles"};
  Uniform<float> _space_boundaries{
      100.f, "spaceLimiter",
      ValueProperties{1.f, 1200.f, Essentials::length_scale, "%.1f mm"}};
  Uniform<float> _bounds_viscosity{0.95f, "boundsViscosity",
                                   ValueProperties{0.f, 1.f, 1.f, "%.2f"}};
  Uniform<float> _global_delta_time{
      1.f, "dt", ValueProperties{1.f, 1000.f, 1e-3f, "%1.f ms"}};
  Uniform<uint32_t> _global_simulation_state{
      static_cast<uint32_t>(Essentials::SimulationState::INIT),
      "SimulatorState"};
  Uniform<uint32_t> _global_world_type{
      static_cast<uint32_t>(Essentials::WorldType::SPHERE_T), "worldType"};
  bool _static_timestep{true};

 private:
  /**
   * @brief Private constructor to avoid creating second instance of a simulator
   *
   */
  Simulator();
  /**
   * @brief Add entity to be tracked by the simulator
   *
   * @tparam T Type of the entity
   * @param entity New entity to be added to the simulator
   * @return Essentials::EntityContainer::key_type
   */
  template <typename T>
  Essentials::EntityContainer::key_type Append(T&& entity);
  /**
   * @brief Method responsible for registering new terrain
   *
   * @param terrain_properties new terrain properties to be added
   * @return uint32_t
   */
  uint32_t AddObstacle(
      Essentials::TerrainBufferProperties const& terrain_properties);
  /**
   * @brief Remove specified entity from a terrain
   *
   * @param id id of the element to remove from terrain
   */
  void RemoveObstacle(Essentials::EntityContainer::key_type id);
  /**
   * @brief Reindex obstacles after removal of a specific obstacle
   *
   * @param removed_id id of the removed obstacle to start reindexing from
   */
  void ReindexObstacles(Essentials::EntityContainer::key_type removed_id);

 public:
  /**
   * @brief Getter allowing to retrieve instance of Simulator
   *
   * @return std::unique_ptr<Simulator>&
   */
  static std::unique_ptr<Simulator>& GetInstance();
  /**
   * @brief Helper method used to safely cleanup instance resources before
   * deleting needed contexts
   *
   */
  static void CleanUp();
  /**
   * @brief Getter allowing to retrieve entities stored within simulator
   * instance
   *
   * @return Essentials::EntityContainer&
   */
  Essentials::EntityContainer& GetEntities();
  /**
   * @brief Getter providing current state of the simulator
   *
   * @return Essentials::SimulationState
   */
  Essentials::SimulationState GetSimulationState();
  /**
   * @brief Method allowing to manually set the simulation time step
   *
   * @param dt
   */
  void UpdateDeltaTime(float dt);
  /**
   * @brief Method responsible for creating graphs out of current simulation
   * frame
   *
   */
  void CreateGraphs();
  /**
   * @brief Get time step of a current simulation frame
   *
   * @return Uniform<float>&
   */
  Uniform<float>& GetDeltaTime();
  /**
   * @brief Getter to flag indicating method for retrieving time step
   *
   * @return true Static time step defined by user is used
   * @return false Dynamic, dependent on the simulation framerate
   */
  bool IsStaticDtUsed();
  /**
   * @brief Helper method used to toggle time step retrieving method
   *
   */
  void ToggleTimesetType();
  /**
   * @brief Method providing Ui configuration for exposed simulator properties
   *
   * @return Essentials::DetailControls
   */
  Essentials::DetailControls GetDetails();
  /**
   * @brief Method responsible for binding simulator-specific uniforms to the
   * provided program
   *
   * @param program_id Id of the program to bound the uniforms to
   */
  void BindUniforms(uint32_t program_id);
  /**
   * @brief Method responsible for binding buffer containing properties of the
   * terrain
   *
   * @param program_id Id of the program to bound the buffer to
   */
  void BindTerrain(uint32_t program_id);
  /**
   * @brief Method used to change internal simulation state
   *
   * @param new_global_state
   */
  void SetSimulationState(Essentials::SimulationState new_global_state);
  /**
   * @brief Method used to create an entity with a given shape
   *
   * @param entity_type Type of the entity to be created
   * @param entity_shape Shape of the entity
   */
  void CreateEntity(Essentials::EntityType entity_type,
                    Essentials::EntityShape entity_shape);
  /**
   * @brief Method used to delete specific entity specified by the unique id
   *
   * @param id unique id of the entity to be deleted
   */
  void Delete(Essentials::EntityContainer::key_type id = 0);
};

template <typename T>
Essentials::EntityContainer::key_type Simulator::Append(T&& entity)
{
  // Register entity to the simulator memory
  Essentials::EntityContainer::key_type entity_id = entity.ID();
  _entitiesContainer[entity_id] = std::make_unique<T>(std::move(entity));
  return entity_id;
}
