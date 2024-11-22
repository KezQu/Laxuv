#pragma once

#include <vector>

#include "Essentials.h"
#include "Program.h"
#include "ShaderStorageBuffer.h"

class PhysicsDispatch
{
 private:
  ShaderStorageBuffer<Essentials::ParticleBufferProperties> _particleMesh;
  std::vector<Program> _physics_runtime_pipeline;
  Program _physics_initialize;
  uint32_t _work_groups{5U};

 public:
  PhysicsDispatch(glm::ivec3 dimensions);
  PhysicsDispatch(PhysicsDispatch const& obj_copy) = delete;
  PhysicsDispatch(PhysicsDispatch&& obj_move) = default;
  PhysicsDispatch& operator=(PhysicsDispatch const& obj_copy) = delete;
  PhysicsDispatch& operator=(PhysicsDispatch&& obj_move) = default;
  ~PhysicsDispatch() = default;

  void Bind() const;
  Program const& GetProgram() const;
  ShaderStorageBuffer<Essentials::ParticleBufferProperties> const&
  GetParticleMeshBuffer() const;
  void UpdateMeshDimensions(glm::uvec3 mesh_radius);
  void InitDefaultShape(glm::uvec3 mesh_radius);
  void Calculate(glm::uvec3 mesh_radius, bool create_snapshot);
};
