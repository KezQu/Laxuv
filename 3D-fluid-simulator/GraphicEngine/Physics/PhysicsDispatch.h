#pragma once

#include <functional>
#include <unordered_map>

#include "Essentials.h"
#include "Program.h"
#include "ShaderStorageBuffer.h"

class PhysicsDispatch
{
 public:
  using bind_callback = std::function<void(uint32_t const& program_id)>;

  PhysicsDispatch(glm::ivec3 dimensions);
  PhysicsDispatch(PhysicsDispatch const& obj_copy) = delete;
  PhysicsDispatch(PhysicsDispatch&& obj_move) = default;
  PhysicsDispatch& operator=(PhysicsDispatch const& obj_copy) = delete;
  PhysicsDispatch& operator=(PhysicsDispatch&& obj_move) = default;
  ~PhysicsDispatch() = default;

  void BindMesh(uint32_t const& program_id) const;
  ShaderStorageBuffer<Essentials::ParticleBufferProperties> const&
  GetParticleMeshBuffer() const;
  void UpdateMeshDimensions(glm::uvec3 mesh_radius);
  void Initialize(glm::uvec3 mesh_radius, bind_callback callback);
  void CalculateFrame(glm::uvec3 mesh_radius, bool create_snapshot,
                      bind_callback callback);

 private:
  ShaderStorageBuffer<Essentials::ParticleBufferProperties> _particleMesh;
  std::unordered_map<std::string, Program> _physics_runtime_pipeline;
  uint32_t _work_groups{1U};

  void ProcessStage(std::string const& stage_name, glm::uvec3 mesh_radius,
                    bind_callback callback);
};
