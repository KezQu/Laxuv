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

  _fluid_properties.eos_factor.MapUniform(_physicsGenerator.ID());
  _fluid_properties.viscosity_factor.MapUniform(_physicsGenerator.ID());
  _fluid_properties.mass.MapUniform(_physicsGenerator.ID());
  _fluid_properties.density0.MapUniform(_physicsGenerator.ID());

  _fluid_properties.influence_kernel.MapUniform(_physicsGenerator.ID());
  _fluid_properties.search_kernel.MapUniform(_physicsGenerator.ID());
  //_fluid_properties.infl_kernel_smoother.MapUniform(_physicsGenerator.ID());

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
  // testing_suite.Init();
}

void PhysicsDispatch::GenerateForces(Essentials::PhysicsType objectPhysicsType)
{
  Bind();
  BindUniforms(objectPhysicsType,
               Simulator::GetInstance().GetSimulationState());
  Calculate(_work_groups, true);

  // Vector Q_n1[Essentials::MaxParticles];
  // for (int i = 0; i < Essentials::MaxParticles; i++)
  //{
  //	Q_n1[i] = testing_suite.GenerateHydrodynamics(i);
  // }
  // for (int i = 0; i < Essentials::MaxParticles; i++)
  //{
  //	testing_suite.particle[i].velocity = glm::vec4(Q_n1[i].y / Q_n1[i].x,
  // 0); 	testing_suite.particle[i].position =
  // testing_suite.particle[i].position
  //+ testing_suite.particle[i].velocity * _dt;
  // }
  // for (int i = 0; i < Essentials::MaxParticles; i++)
  //{
  //	testing_suite.FindNeighbours(i, Essentials::MaxParticles);
  // }
  // for (int i = 0; i < Essentials::MaxParticles; i++)
  //{
  //	const float volume = 1. / testing_suite.CalculateOmega(i);
  //	int n = 0;
  //	while (testing_suite.particle[i].neighbours[n] != 0xffff) n++;
  //	const float pressure = testing_suite.R * (n + 1) * 273.f / volume;
  //	testing_suite.particle[i].VolumeDensityPressureMass.x = volume;
  //	testing_suite.particle[i].VolumeDensityPressureMass.y = Q_n1[i].x /
  // volume; 	testing_suite.particle[i].VolumeDensityPressureMass.z =
  // pressure; 	testing_suite.particle[i].VolumeDensityPressureMass.w =
  // Q_n1[i].x;
  // }
}

void PhysicsDispatch::UpdateDeltaTime()
{
  _dt = 1.f / ImGui::GetIO().Framerate;
  //_dt = 0.0001f;
}

Uniform<float>& PhysicsDispatch::GetDeltaTime()
{
  return _dt;
}
