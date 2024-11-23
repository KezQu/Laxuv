#include "PhysicsDispatch.h"

#include <chrono>
#include <cmath>
#include <cstdint>

#include "Debug.h"
#include "Essentials.h"
#include "GL/glew.h"
#include "Program.h"
#include "ShaderStorageBuffer.h"
#include "glm/fwd.hpp"

PhysicsDispatch::PhysicsDispatch(glm::ivec3 dimensions)
    : _particleMesh{"dataBuffer",
                    static_cast<uint64_t>(dimensions.x * dimensions.y *
                                          dimensions.z)},
      _mesh_granularity{"granularityBuffer"},
      _max_particles{0, "MaxParticles"}
{
  _physics_runtime_pipeline["Init"] =
      Program{{GL_COMPUTE_SHADER, "/Initialize.comp"},
              {GL_COMPUTE_SHADER, "/EvaluateColor.glsl"},
              {GL_COMPUTE_SHADER, "/InitHelpers.glsl"},
              {GL_COMPUTE_SHADER, "/HydroHelperFunctions.glsl"}};
  _physics_runtime_pipeline["Runtime"] =
      Program{{GL_COMPUTE_SHADER, "/Runtime.comp"},
              {GL_COMPUTE_SHADER, "/EvaluateColor.glsl"},
              {GL_COMPUTE_SHADER, "/HydroHelperFunctions.glsl"}};
  std::for_each(_physics_runtime_pipeline.begin(),
                _physics_runtime_pipeline.end(),
                [](auto& program)
                {
                  if (!program.second.isLinked()) program.second.Link();
                });
}

void PhysicsDispatch::BindPhysicsMesh(uint32_t const& program_id) const
{
  _particleMesh.Bind(program_id);
}

ShaderStorageBuffer<Essentials::ParticleBufferProperties> const&
PhysicsDispatch::GetParticleMeshBuffer() const
{
  return _particleMesh;
}

void PhysicsDispatch::UpdateMeshDimensions(glm::ivec3 mesh_radius)
{
  _max_particles = mesh_radius.x * mesh_radius.y * mesh_radius.z;
  if (_particleMesh.Size() != _max_particles.GetValue())
  {
    _particleMesh.SetBufferMemorySize(_max_particles.GetValue());
  }
}

void PhysicsDispatch::CalculateFrame(glm::ivec3 mesh_radius,
                                     bool create_snapshot,
                                     PhysicsDispatch::bind_callback callback)
{
  ProcessStage("Runtime", mesh_radius, callback);
}

void PhysicsDispatch::Initialize(glm::ivec3 mesh_radius,
                                 PhysicsDispatch::bind_callback callback)
{
  UpdateMeshDimensions(mesh_radius);
  UpdateGranularityMesh(mesh_radius);

  ProcessStage("Init", mesh_radius, callback);
}

void PhysicsDispatch::ProcessStage(std::string const& stage_name,
                                   glm::ivec3 mesh_radius,
                                   bind_callback callback)
{
  _physics_runtime_pipeline[stage_name].Bind();
  _max_particles.MapUniform(_physics_runtime_pipeline[stage_name].ID());
  _mesh_granularity.Bind(_physics_runtime_pipeline[stage_name].ID());
  BindPhysicsMesh(_physics_runtime_pipeline[stage_name].ID());
  callback(_physics_runtime_pipeline[stage_name].ID());

  _(glDispatchCompute(1, 1, 1));
  _(glMemoryBarrier(GL_ALL_BARRIER_BITS));
  glFinish();
}

void PhysicsDispatch::UpdateGranularityMesh(glm::ivec3 mesh_radius)
{
  uint32_t calculation_mesh_size_flat =
      _work_groups * _work_groups * _work_groups;
  std::vector<PhysicsDispatch::granularity_t> mesh_division(
      calculation_mesh_size_flat);
  std::for_each(
      mesh_division.begin(), mesh_division.end(), [](auto& instances)
      { std::fill(std::begin(instances), std::end(instances), 0xffffffff); });
  for (int i = 0; i < _max_particles.GetValue(); i++)
  {
    mesh_division[i % calculation_mesh_size_flat]
                 [i / calculation_mesh_size_flat] = i;
  }
  _mesh_granularity.UpdateBufferMemory(std::move(mesh_division));
}