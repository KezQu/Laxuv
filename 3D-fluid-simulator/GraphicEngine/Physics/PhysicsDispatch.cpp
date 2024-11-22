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
    : _particleMesh{
          "dataBuffer",
          static_cast<uint64_t>(dimensions.x * dimensions.y * dimensions.z)}
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
  // _physics_runtime_pipeline["WorldCoord"] =
  //     Program{{GL_COMPUTE_SHADER, "/TranslateToWorldCoords.comp"}};
  // _physics_runtime_pipeline["ExtForces"] =
  //     Program{{GL_COMPUTE_SHADER, "/CalculateExternalForces.comp"},
  //             {GL_COMPUTE_SHADER, "/HydroHelperFunctions.glsl"}};
  // _physics_runtime_pipeline["GetDen"] =
  //     Program{{GL_COMPUTE_SHADER, "/GetDensity.comp"},
  //             {GL_COMPUTE_SHADER, "/HydroHelperFunctions.glsl"}};
  // _physics_runtime_pipeline["CorrVel"] =
  //     Program{{GL_COMPUTE_SHADER, "/CorrectVelocity.comp"},
  //             {GL_COMPUTE_SHADER, "/HydroHelperFunctions.glsl"}};
  // _physics_runtime_pipeline["UptPos"] =
  //     Program{{GL_COMPUTE_SHADER, "/UpdatePositions.comp"},
  //             {GL_COMPUTE_SHADER, "/HydroHelperFunctions.glsl"}};
  // _physics_runtime_pipeline["FindNeigh"] =
  //     Program{{GL_COMPUTE_SHADER, "/FindNeighbours.comp"},
  //             {GL_COMPUTE_SHADER, "/HydroHelperFunctions.glsl"}};
  // _physics_runtime_pipeline["UptProp"] =
  //     Program{{GL_COMPUTE_SHADER, "/UpdateProperties.comp"},
  //             {GL_COMPUTE_SHADER, "/HydroHelperFunctions.glsl"}};
  // _physics_runtime_pipeline["DivErr"] =
  //     Program{{GL_COMPUTE_SHADER, "/DivergenceError.comp"},
  //             {GL_COMPUTE_SHADER, "/HydroHelperFunctions.glsl"}};
  // _physics_runtime_pipeline["CheckColl"] =
  //     Program{{GL_COMPUTE_SHADER, "/CheckCollisions.comp"},
  //             {GL_COMPUTE_SHADER, "/HydroHelperFunctions.glsl"}};
  // _physics_runtime_pipeline["ModelCoord"] =
  //     Program{{GL_COMPUTE_SHADER, "/TranslateToModelCoords.comp"},
  //             {GL_COMPUTE_SHADER, "/EvaluateColor.glsl"}};
  std::for_each(_physics_runtime_pipeline.begin(),
                _physics_runtime_pipeline.end(),
                [](auto& program)
                {
                  if (!program.second.isLinked()) program.second.Link();
                });
}

void PhysicsDispatch::BindMesh(uint32_t const& program_id) const
{
  _particleMesh.Bind(program_id);
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

void PhysicsDispatch::CalculateFrame(glm::uvec3 mesh_radius,
                                     bool create_snapshot,
                                     PhysicsDispatch::bind_callback callback)
{
  _physics_runtime_pipeline["Runtime"].Bind();

  callback(_physics_runtime_pipeline["Runtime"].ID());
  BindMesh(_physics_runtime_pipeline["Runtime"].ID());
  physics_stage = static_cast<uint32_t>(PhysicsStage::WORLD_COORD);
  ProcessStage("Runtime", mesh_radius, callback);
  physics_stage = static_cast<uint32_t>(PhysicsStage::EXT_FORCES);
  ProcessStage("Runtime", mesh_radius, callback);
  for (int i = 0; i < 1000; i++)
  {
    physics_stage = static_cast<uint32_t>(PhysicsStage::GET_DEN);
    ProcessStage("Runtime", mesh_radius, callback);
    physics_stage = static_cast<uint32_t>(PhysicsStage::CORR_VEL);
    ProcessStage("Runtime", mesh_radius, callback);
  }
  physics_stage = static_cast<uint32_t>(PhysicsStage::UPT_POS);
  ProcessStage("Runtime", mesh_radius, callback);
  physics_stage = static_cast<uint32_t>(PhysicsStage::FIND_NEIGH);
  ProcessStage("Runtime", mesh_radius, callback);
  physics_stage = static_cast<uint32_t>(PhysicsStage::UPT_PROP);
  ProcessStage("Runtime", mesh_radius, callback);
  for (int i = 0; i < 1000; i++)
  {
    physics_stage = static_cast<uint32_t>(PhysicsStage::DIV_ERR);
    ProcessStage("Runtime", mesh_radius, callback);
    physics_stage = static_cast<uint32_t>(PhysicsStage::CORR_VEL);
    ProcessStage("Runtime", mesh_radius, callback);
  }
  physics_stage = static_cast<uint32_t>(PhysicsStage::CHECK_COLL);
  ProcessStage("Runtime", mesh_radius, callback);
  physics_stage = static_cast<uint32_t>(PhysicsStage::MODEL_COORD);
  ProcessStage("Runtime", mesh_radius, callback);
  // if (create_snapshot)
  // {
  //   auto lookupBuffer =
  //       _particleMesh.GetBufferSubData(0U, _particleMesh.Size());
  // }
}

void PhysicsDispatch::Initialize(glm::uvec3 mesh_radius,
                                 PhysicsDispatch::bind_callback callback)
{
  UpdateMeshDimensions(mesh_radius);
  _physics_runtime_pipeline["Init"].Bind();

  callback(_physics_runtime_pipeline["Init"].ID());
  BindMesh(_physics_runtime_pipeline["Init"].ID());
  physics_stage = static_cast<uint32_t>(PhysicsStage::NONE);
  ProcessStage("Init", mesh_radius, callback);
}

void PhysicsDispatch::ProcessStage(std::string const& stage_name,
                                   glm::uvec3 mesh_radius,
                                   bind_callback callback)
{
  std::chrono::high_resolution_clock::time_point start =
      std::chrono::high_resolution_clock::now();
  physics_stage.MapUniform(_physics_runtime_pipeline[stage_name].ID());

  std::chrono::high_resolution_clock::time_point stop =
      std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed_time = stop - start;
  std::cout << "Elapsed time for is " << elapsed_time.count() << "\n";
  _(glDispatchCompute(mesh_radius.x, mesh_radius.y, mesh_radius.z));
  _(glMemoryBarrier(GL_ALL_BARRIER_BITS));
  glFinish();
  start = std::chrono::high_resolution_clock::now();
  elapsed_time = start - stop;
  std::cout << "--Elapsed time for is " << elapsed_time.count() << "\n";
}