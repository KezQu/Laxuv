#pragma once

#include <HydroTest.h>
#include <Uniform.h>

#include "Essentials.h"
#include "Program.h"
#include "ShaderStorageBuffer.h"

class PhysicsDispatch
{
 private:
  ShaderStorageBuffer<Essentials::ParticleProperties> _particleMesh;
  Essentials::FluidProperties _fluid_properties;
  Program _physicsGenerator;
  // HydroTest testing_suite{};
  uint32_t _work_groups{1U};

 private:
  void Bind() const;
  void BindUniforms(Essentials::PhysicsType object_physics_type) const;
  void BindExternalUniforms(
      Essentials::ShapeProperties const& shape_properties) const;

 public:
  PhysicsDispatch(glm::ivec3 dimensions);
  PhysicsDispatch(PhysicsDispatch const& obj_copy) = delete;
  PhysicsDispatch(PhysicsDispatch&& obj_move) = default;
  PhysicsDispatch& operator=(PhysicsDispatch const& obj_copy) = delete;
  PhysicsDispatch& operator=(PhysicsDispatch&& obj_move) = default;
  ~PhysicsDispatch() = default;

  ShaderStorageBuffer<Essentials::ParticleProperties> const&
  GetParticleMeshBuffer() const;
  Essentials::FluidProperties& GetFluidProperties();
  void UpdateMeshDimensions();
  void InitDefaultShape(Essentials::PhysicsType object_physics_type,
                        Essentials::ShapeProperties const& shape_properties);
  void GenerateForces(Essentials::ShapeProperties const& shape_properties,
                      Essentials::PhysicsType object_physics_type);
  void Calculate(uint32_t work_groups, bool create_snapshot);
};
