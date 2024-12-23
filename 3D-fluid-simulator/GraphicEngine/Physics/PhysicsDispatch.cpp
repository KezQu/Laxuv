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
  // Initialize pipeline responsible for setting entities to default state
  _physics_runtime_pipeline["Init"] =
      Program{{GL_COMPUTE_SHADER, "/Initialize.comp"},
              {GL_COMPUTE_SHADER, "/EvaluateColor.glsl"},
              {GL_COMPUTE_SHADER, "/InitHelpers.glsl"},
              {GL_COMPUTE_SHADER, "/HydroHelperFunctions.glsl"}};
  // Initialize pipeline responsible for calculation simulation frames during
  // runtime
  _physics_runtime_pipeline["Runtime"] =
      Program{{GL_COMPUTE_SHADER, "/Runtime.comp"},
              {GL_COMPUTE_SHADER, "/EvaluateColor.glsl"},
              {GL_COMPUTE_SHADER, "/HydroHelperFunctions.glsl"}};
  // Compile and link created pipelines
  std::for_each(_physics_runtime_pipeline.begin(),
                _physics_runtime_pipeline.end(), [](auto& program) {
                  if (!program.second.isLinked()) program.second.Link();
                });
}

void PhysicsDispatch::BindPhysicsMesh(uint32_t const& program_id) const
{
  // Bind buffer containing info about entities to a given program
  _particleMesh.Bind(program_id);
}

ShaderStorageBuffer<Essentials::ParticleBufferProperties> const&
PhysicsDispatch::GetParticleMeshBuffer() const
{
  return _particleMesh;
}

void PhysicsDispatch::UpdateMeshDimensions(glm::ivec3 mesh_size)
{
  // Resize particle properties buffer based on a requested new mesh size
  _max_particles = mesh_size.x * mesh_size.y * mesh_size.z;
  if (_particleMesh.Size() != _max_particles.GetValue())
  {
    _particleMesh.SetBufferMemorySize(_max_particles.GetValue());
  }
}

void PhysicsDispatch::CalculateFrame(glm::ivec3 mesh_size,
                                     PhysicsDispatch::bind_callback callback)
{
  // Schedule calculating a simulation frame pipeline
  ProcessStage("Runtime", callback);
}

void PhysicsDispatch::Initialize(glm::ivec3 mesh_size,
                                 PhysicsDispatch::bind_callback callback)
{
  // Resize properties buffer
  UpdateMeshDimensions(mesh_size);
  // Optimize workload
  UpdateGranularityMesh();

  // Schedule calculating a initialization pipeline
  ProcessStage("Init", callback);
}

void PhysicsDispatch::ProcessStage(std::string const& stage_name,
                                   bind_callback callback)
{
  // Bind needed uniforms and buffers and execute binding callback
  _physics_runtime_pipeline[stage_name].Bind();
  _max_particles.MapUniform(_physics_runtime_pipeline[stage_name].ID());
  _mesh_granularity.Bind(_physics_runtime_pipeline[stage_name].ID());
  BindPhysicsMesh(_physics_runtime_pipeline[stage_name].ID());
  callback(_physics_runtime_pipeline[stage_name].ID());

  // Request OpenGL to calculate a single workgroup responsible for physics
  _(glDispatchCompute(1, 1, 1));
  // Await OpenGL pipeline execution
  _(glMemoryBarrier(GL_ALL_BARRIER_BITS));
  glFinish();
}

void PhysicsDispatch::UpdateGranularityMesh()
{
  // Flatten work group size to further optimize workload
  uint32_t calculation_mesh_size_flat =
      _work_groups * _work_groups * _work_groups;
  // Initialize raw data used for providing information where given objects and
  // particles are calculated
  std::vector<PhysicsDispatch::granularity_t> mesh_division(
      calculation_mesh_size_flat);
  // Set data with a stop flag
  std::for_each(
      mesh_division.begin(), mesh_division.end(), [](auto& instances) {
        std::fill(std::begin(instances), std::end(instances), 0xffffffff);
      });
  // For every particle assign its id to specific invocation inside a work group
  for (int i = 0; i < _max_particles.GetValue(); i++)
  {
    mesh_division[i % calculation_mesh_size_flat]
                 [i / calculation_mesh_size_flat] = i;
  }
  // Update helper buffer specifying order of processing inside work group with
  // new order1`
  _mesh_granularity.UpdateBufferMemory(std::move(mesh_division));
}