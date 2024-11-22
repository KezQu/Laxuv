#include "PhysicsDispatch.h"

#include <cmath>
#include <cstdint>

#include "Debug.h"
#include "Essentials.h"
#include "GL/glew.h"
#include "ShaderStorageBuffer.h"
#include "glm/fwd.hpp"

PhysicsDispatch::PhysicsDispatch(glm::ivec3 dimensions)
    : _particleMesh{"dataBuffer",
                    static_cast<uint64_t>(dimensions.x * dimensions.y *
                                          dimensions.z)},
      _physics_runtime_pipeline{
          Program{{GL_COMPUTE_SHADER, "/TranslateToWorldCoords.comp"}},
          Program{{GL_COMPUTE_SHADER, "/CalculateExternalForces.comp"},
                  {GL_COMPUTE_SHADER, "/HydroHelperFunctions.comp"}},
          Program{{GL_COMPUTE_SHADER, "/DensityError.comp"},
                  {GL_COMPUTE_SHADER, "/HydroHelperFunctions.comp"}},
          Program{{GL_COMPUTE_SHADER, "/FindNeighbours.comp"},
                  {GL_COMPUTE_SHADER, "/HydroHelperFunctions.comp"}},
          Program{{GL_COMPUTE_SHADER, "/DivergenceError.comp"},
                  {GL_COMPUTE_SHADER, "/HydroHelperFunctions.comp"}},
          Program{{GL_COMPUTE_SHADER, "/CheckCollisions.comp"},
                  {GL_COMPUTE_SHADER, "/HydroHelperFunctions.comp"}},
          Program{{GL_COMPUTE_SHADER, "/TranslateToModelCoords.comp"}}},
      _physics_initialize{{GL_COMPUTE_SHADER, "/Initialize.comp"},
                          {GL_COMPUTE_SHADER, "/InitHelpers.glsl"},
                          {GL_COMPUTE_SHADER, "/HydroHelperFunctions.glsl"}}
{
}

void PhysicsDispatch::Bind() const
{
  if (!_physicsGenerator.isLinked()) _physicsGenerator.Link();
  _physicsGenerator.Bind();
  _particleMesh.Bind(_physicsGenerator.ID());
}

Program const& PhysicsDispatch::GetProgram() const
{
  return _physicsGenerator;
}

ShaderStorageBuffer<Essentials::ParticleBufferProperties> const&
PhysicsDispatch::GetParticleMeshBuffer() const
{
  return _particleMesh;
}

void PhysicsDispatch::UpdateMeshDimensions(glm::uvec3 mesh_radius)
{
  glm::uvec3 rescaled_mesh = mesh_radius * _work_groups;
  uint32_t new_buffer_size =
      rescaled_mesh.x * rescaled_mesh.y * rescaled_mesh.z;
  if (_particleMesh.Size() != new_buffer_size)
  {
    _particleMesh.SetBufferMemorySize(new_buffer_size);
  }
}

void PhysicsDispatch::Calculate(glm::uvec3 mesh_radius, bool create_snapshot)
{
  _(glDispatchCompute(mesh_radius.x, mesh_radius.y, mesh_radius.z));
  _(glMemoryBarrier(GL_ALL_BARRIER_BITS));
  glFinish();
  if (create_snapshot)
  {
    auto lookupBuffer =
        _particleMesh.GetBufferSubData(0U, _particleMesh.Size());
  }
}

void PhysicsDispatch::InitDefaultShape(glm::uvec3 mesh_radius)
{
  UpdateMeshDimensions(mesh_radius);
  Calculate(mesh_radius, true);
}
