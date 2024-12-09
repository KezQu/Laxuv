#pragma once

#include <array>
#include <functional>
#include <unordered_map>

#include "CPUBuffer.h"
#include "Essentials.h"
#include "Program.h"
#include "ShaderStorageBuffer.h"
#include "Uniform.h"

enum class PhysicsStage : uint32_t
{
  NONE,
  WORLD_COORD,
  EXT_FORCES,
  GET_DEN,
  CORR_VEL,
  UPT_POS,
  FIND_NEIGH,
  UPT_PROP,
  DIV_ERR,
  CHECK_COLL,
  MODEL_COORD
};
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

  void BindPhysicsMesh(uint32_t const& program_id) const;
  ShaderStorageBuffer<Essentials::ParticleBufferProperties> const&
  GetParticleMeshBuffer() const;
  void UpdateMeshDimensions(glm::ivec3 mesh_radius);
  void Initialize(glm::ivec3 mesh_radius, bind_callback callback);
  void CalculateFrame(glm::ivec3 mesh_radius, bind_callback callback);

 private:
  using granularity_t = uint32_t[16];

  ShaderStorageBuffer<Essentials::ParticleBufferProperties> _particleMesh;
  ShaderStorageBuffer<granularity_t, CPUBuffer> _mesh_granularity;
  std::unordered_map<std::string, Program> _physics_runtime_pipeline;
  Uniform<uint32_t> _max_particles;
  uint32_t _work_groups{10U};

  void ProcessStage(std::string const& stage_name, glm::ivec3 mesh_radius,
                    bind_callback callback);
  void UpdateGranularityMesh(glm::ivec3 mesh_radius);
};
