#pragma once

#include <array>
#include <functional>
#include <unordered_map>

#include "CPUBuffer.h"
#include "Essentials.h"
#include "Program.h"
#include "ShaderStorageBuffer.h"
#include "Uniform.h"

/**
 * @brief Class responsible for scheduling calculation of physics to the GPU
 * using OpenGL Interface
 *
 */
class PhysicsDispatch
{
 public:
  using bind_callback = std::function<void(uint32_t const& program_id)>;

  /**
   * @brief Initialize physics handler for a given mesh size
   *
   * @param dimensions request calculation mesh size
   */
  PhysicsDispatch(glm::ivec3 dimensions);
  PhysicsDispatch(PhysicsDispatch const& obj_copy) = delete;
  PhysicsDispatch(PhysicsDispatch&& obj_move) = default;
  PhysicsDispatch& operator=(PhysicsDispatch const& obj_copy) = delete;
  PhysicsDispatch& operator=(PhysicsDispatch&& obj_move) = default;
  ~PhysicsDispatch() = default;

  /**
   * @brief Buffer providing necessary information about particles to calculate
   * single simulation frame
   *
   * @param program_id id of a program to bind the buffer to
   */
  void BindPhysicsMesh(uint32_t const& program_id) const;
  /**
   * @brief Getter exposing particles buffer in order to retrieve data about
   * them (read-only)
   *
   * @return ShaderStorageBuffer<Essentials::ParticleBufferProperties> const&
   */
  ShaderStorageBuffer<Essentials::ParticleBufferProperties> const&
  GetParticleMeshBuffer() const;
  /**
   * @brief Update calculation mesh with a provided mesh size
   *
   * @param mesh_size
   */
  void UpdateMeshDimensions(glm::ivec3 mesh_size);
  /**
   * @brief Method allowing to run the initialization pipeline
   *
   * @param mesh_size Mesh size to be updated to equal the workload inside a
   * work group
   * @param callback Callback allowing to bind additional uniforms and buffer to
   * the program
   */
  void Initialize(glm::ivec3 mesh_size, bind_callback callback);
  void CalculateFrame(glm::ivec3 mesh_size, bind_callback callback);

 private:
  using granularity_t = uint32_t[16];

  ShaderStorageBuffer<Essentials::ParticleBufferProperties> _particleMesh;
  ShaderStorageBuffer<granularity_t, CPUBuffer> _mesh_granularity;
  std::unordered_map<std::string, Program> _physics_runtime_pipeline;
  Uniform<uint32_t> _max_particles;
  uint32_t _work_groups{10U};

  /**
   * @brief Request OpenGL to execute provided pipeline with previously
   * registered uniforms and buffers from callback
   *
   * @param stage_name Stage name to be processed
   * @param callback Callback allowing to bind additional uniforms and buffer to
   * the program
   */
  void ProcessStage(std::string const& stage_name, bind_callback callback);
  /**
   * @brief Helper method used to optimize workload inside a work group
   *
   */
  void UpdateGranularityMesh();
};
