#include "PhysicsDispatch.h"

#include <cmath>
#include <cstdint>

#include "Debug.h"
#include "Essentials.h"
#include "GL/glew.h"
#include "ShaderStorageBuffer.h"
#include "Simulator.h"
#include "Uniform.h"
#include "glm/fwd.hpp"

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
    Essentials::PhysicsType object_physics_type) const
{
  Uniform<uint32_t> physics_type{static_cast<uint32_t>(object_physics_type),
                                 "physicsType"};

  physics_type.MapUniform(_physicsGenerator.ID());

  _fluid_properties.viscosity_factor.MapUniform(_physicsGenerator.ID());
  _fluid_properties.mass.MapUniform(_physicsGenerator.ID());

  _fluid_properties.kernel_a.MapUniform(_physicsGenerator.ID());
  _fluid_properties.influence_kernel.MapUniform(_physicsGenerator.ID());
  _fluid_properties.search_kernel.MapUniform(_physicsGenerator.ID());

  _fluid_properties.particle_spacing.MapUniform(_physicsGenerator.ID());
  _fluid_properties.distribution_shape.MapUniform(_physicsGenerator.ID());
}

void PhysicsDispatch::BindExternalUniforms(
    Essentials::ShapeProperties const& shape_properties) const
{
  Simulator::GetInstance().BindUniforms(_physicsGenerator.ID());
  shape_properties.Model().MapUniform(_physicsGenerator.ID());
  shape_properties._radius.MapUniform(_physicsGenerator.ID());
  shape_properties._color.first.MapUniform(_physicsGenerator.ID());
  shape_properties._color.second.MapUniform(_physicsGenerator.ID());
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
  _(glMemoryBarrier(GL_ALL_BARRIER_BITS));
  glFinish();
  if (create_snapshot)
  {
    auto lookupBuffer =
        _particleMesh.GetBufferSubData(0U, _particleMesh.Size());
  }
}

void PhysicsDispatch::InitDefaultShape(
    Essentials::PhysicsType object_physics_type,
    Essentials::ShapeProperties const& shape_properties)
{
  UpdateMeshDimensions();
  Bind();
  BindUniforms(object_physics_type);
  BindExternalUniforms(shape_properties);
  Calculate(_work_groups, true);
}

void PhysicsDispatch::GenerateForces(
    Essentials::ShapeProperties const& shape_properties,
    Essentials::PhysicsType object_physics_type)
{
  Bind();
  BindUniforms(object_physics_type);
  BindExternalUniforms(shape_properties);

  Calculate(_work_groups, false);
}
