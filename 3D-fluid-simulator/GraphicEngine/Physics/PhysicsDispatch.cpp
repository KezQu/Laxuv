#include "PhysicsDispatch.h"

#include <Simulator.h>
#include <imgui.h>

Uniform<float> PhysicsDispatch::_dt{0.f, "dt"};

PhysicsDispatch::PhysicsDispatch(glm::ivec3 dimensions)
    : _meshDimensions{dimensions},
      _particleMesh{
          static_cast<uint64_t>(dimensions.x * dimensions.y * dimensions.z)}
{
  _physicsGenerator.AddShader(GL_COMPUTE_SHADER, "/Element.comp");
  _physicsGenerator.AddShader(GL_COMPUTE_SHADER, "/InitDefaultShape.glsl");
  _physicsGenerator.AddShader(GL_COMPUTE_SHADER, "/Gravitation.glsl");
  _physicsGenerator.AddShader(GL_COMPUTE_SHADER, "/Hydrodynamics.glsl");
}

void PhysicsDispatch::Bind() const
{
  if (!_physicsGenerator.isLinked()) _physicsGenerator.Link();
  _physicsGenerator.Bind();
  _particleMesh.Bind(_physicsGenerator.ID());
}

void PhysicsDispatch::BindUniforms(
    Essentials::DistributionShape initObjectBounds,
    Essentials::PhysicsType objectPhysicsType, uint32_t shapeRadius) const
{
  Uniform<uint32_t> simulation_state{
      static_cast<uint32_t>(Simulator::GetInstance().GetSimulationState()),
      "SimulatorState"};
  Uniform<uint32_t> distribution_shape{static_cast<int32_t>(initObjectBounds),
                                       "DistributionShape"};
  Uniform<uint32_t> physics_type{static_cast<uint32_t>(objectPhysicsType),
                                 "physicsType"};
  Uniform<uint32_t> shape_radius{static_cast<uint32_t>(shapeRadius),
                                 "shapeRadius"};

  simulation_state.MapUniform(_physicsGenerator.ID());
  distribution_shape.MapUniform(_physicsGenerator.ID());
  physics_type.MapUniform(_physicsGenerator.ID());
  shape_radius.MapUniform(_physicsGenerator.ID());
  _fluidProperties.gamma.MapUniform(_physicsGenerator.ID());
  _fluidProperties.kernel_radius.MapUniform(_physicsGenerator.ID());
  _dt.MapUniform(_physicsGenerator.ID());
}

ShaderStorageBuffer<Essentials::PhysicsProperties> const&
PhysicsDispatch::GetParticleMeshBuffer() const
{
  return _particleMesh;
}

glm::uvec3 const& PhysicsDispatch::GetMeshDimensions() const
{
  return _meshDimensions;
}

void PhysicsDispatch::UpdateMeshDimensions(glm::uvec3 dimensions)
{
  if (_meshDimensions != dimensions)
  {
    _meshDimensions = dimensions;
    _meshDimensions = dimensions * 10U;
    _particleMesh.SetBufferMemorySize(_meshDimensions.x * _meshDimensions.y *
                                      _meshDimensions.z);
  }
}

void PhysicsDispatch::InitDefaultShape(
    Essentials::DistributionShape initObjectBounds,
    Essentials::PhysicsType objectPhysicsType, uint32_t shapeRadius)
{
  Bind();
  BindUniforms(initObjectBounds, objectPhysicsType, shapeRadius);
  _(glDispatchCompute(_meshDimensions.x / 10, _meshDimensions.y / 10,
                      _meshDimensions.z / 10));
  //_(glDispatchCompute(_meshDimensions.x, _meshDimensions.y,
  //_meshDimensions.z));
  _(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT));
  auto lookupBuffer = _particleMesh.GetBufferSubData(
      0U, _meshDimensions.x * _meshDimensions.y * _meshDimensions.z);

  // for (auto& particle : lookupBuffer) {
  //	std::cout << particle;
  // }
  // for (int i = 0; i < lookupBuffer.size(); i++) {
  //	if (lookupBuffer[i].position.x != _particleMesh.Size()) {
  //		std::cout << lookupBuffer[i-1].position.x;
  //		std::cout << lookupBuffer[i].position.x;
  //		__debugbreak();
  //	}
  // }
  // testing_suite.Init();
}

void PhysicsDispatch::GenerateForces(Essentials::PhysicsType objectPhysicsType)
{
  Bind();
  BindUniforms(Essentials::DistributionShape::QUBE, objectPhysicsType, 2);

  _(glDispatchCompute(_meshDimensions.x / 10., _meshDimensions.y / 10.,
                      _meshDimensions.z / 10.));
  //_(glDispatchCompute(_meshDimensions.x, _meshDimensions.y,
  //_meshDimensions.z));
  _(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT));
  auto lookupBuffer = _particleMesh.GetBufferSubData(
      0U, _meshDimensions.x * _meshDimensions.y * _meshDimensions.z);

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
  //_dt = 1.f / ImGui::GetIO().Framerate;
  _dt = 0.01f;
}
