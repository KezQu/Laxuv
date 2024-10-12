#include "PhysicsDispatch.h"

#include <Simulator.h>
#include <imgui.h>

Uniform<float> PhysicsDispatch::_dt{0.f, "dt"};

PhysicsDispatch::PhysicsDispatch(glm::ivec3 dimensions)
    : _particleMesh{
          "dataBuffer",
          static_cast<uint64_t>(dimensions.x * dimensions.y * dimensions.z)}
{
  _physicsGenerator.AddShader(GL_COMPUTE_SHADER, "/Element.comp");
  _physicsGenerator.AddShader(GL_COMPUTE_SHADER, "/InitDefaultShape.glsl");
  _physicsGenerator.AddShader(GL_COMPUTE_SHADER, "/Hydrodynamics.glsl");
}

void PhysicsDispatch::Bind() const
{
  if (!_physicsGenerator.isLinked()) _physicsGenerator.Link();
  _physicsGenerator.Bind();
  _particleMesh.Bind(_physicsGenerator.ID());
}

void PhysicsDispatch::BindUniforms(
    Essentials::PhysicsType objectPhysicsType,
    Essentials::SimulationState current_sim_state) const
{
  Uniform<uint32_t> simulation_state{static_cast<uint32_t>(current_sim_state),
                                     "SimulatorState"};
  Uniform<uint32_t> physics_type{static_cast<uint32_t>(objectPhysicsType),
                                 "physicsType"};

  simulation_state.MapUniform(_physicsGenerator.ID());
  physics_type.MapUniform(_physicsGenerator.ID());

  _fluid_properties.viscosity_factor.MapUniform(_physicsGenerator.ID());
  _fluid_properties.mass.MapUniform(_physicsGenerator.ID());
  _fluid_properties.density0.MapUniform(_physicsGenerator.ID());

  _fluid_properties.kernel_a.MapUniform(_physicsGenerator.ID());
  _fluid_properties.influence_kernel.MapUniform(_physicsGenerator.ID());
  _fluid_properties.search_kernel.MapUniform(_physicsGenerator.ID());

  _fluid_properties.particle_radius.MapUniform(_physicsGenerator.ID());
  _fluid_properties.particle_spacing.MapUniform(_physicsGenerator.ID());
  _fluid_properties.space_limiter.MapUniform(_physicsGenerator.ID());
  _fluid_properties.bounds_viscosity.MapUniform(_physicsGenerator.ID());
  _fluid_properties.distribution_shape.MapUniform(_physicsGenerator.ID());

  _dt.MapUniform(_physicsGenerator.ID());
}

ShaderStorageBuffer<Essentials::ParticleProperties> const&
PhysicsDispatch::GetParticleMeshBuffer() const
{
  return _particleMesh;
}

Essentials::FluidProperties& PhysicsDispatch::GetFluidProperties()
{
  return _fluid_properties;
}

void PhysicsDispatch::UpdateMeshDimensions()
{
  uint64_t new_buffer_size =
      std::pow(_fluid_properties.mesh_radius * _work_groups, 3);
  if (_particleMesh.Size() != new_buffer_size)
  {
    _particleMesh.SetBufferMemorySize(new_buffer_size);
  }
}

void PhysicsDispatch::Calculate(uint32_t work_groups, bool create_snapshot)
{
  _(glDispatchCompute(_fluid_properties.mesh_radius,
                      _fluid_properties.mesh_radius,
                      _fluid_properties.mesh_radius));
  _(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT));
  if (create_snapshot)
  {
    auto lookupBuffer =
        _particleMesh.GetBufferSubData(0U, _particleMesh.Size());
  }
}

void PhysicsDispatch::InitDefaultShape(
    Essentials::PhysicsType objectPhysicsType, uint32_t particleRadius)
{
  _fluid_properties.particle_radius = particleRadius;
  UpdateMeshDimensions();
  Bind();
  BindUniforms(objectPhysicsType, Essentials::SimulationState::INIT);
  Calculate(_work_groups, true);

  uint32_t MaxParticles =
      std::pow(_fluid_properties.mesh_radius * _work_groups, 3);
  // testing_suite.Init(_fluid_properties.mass.GetValue(),
  //                    _fluid_properties.particle_radius.GetValue(),
  //                    _fluid_properties.particle_spacing.GetValue(),
  //                    MaxParticles,
  //                    _fluid_properties.influence_kernel.GetValue(),
  //                    _fluid_properties.search_kernel.GetValue(),
  //                    _fluid_properties.kernel_a.GetValue());
}

void PhysicsDispatch::GenerateForces(Essentials::PhysicsType objectPhysicsType)
{
  Bind();
  BindUniforms(objectPhysicsType,
               Simulator::GetInstance().GetSimulationState());
  Calculate(_work_groups, false);
  // testing_suite.CalculateDFSPHSimulation(
  //     std::pow(_fluid_properties.mesh_radius * _work_groups, 3), 0,
  //     // 9.807f, _fluid_properties.density0.GetValue());
}

void PhysicsDispatch::UpdateDeltaTime()
{
  // _dt = 1.f / ImGui::GetIO().Framerate;
  _dt = 0.016f;
}

Uniform<float>& PhysicsDispatch::GetDeltaTime()
{
  return _dt;
}
