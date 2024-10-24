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
  uint32_t _work_groups{10U};

 private:
  void Bind() const;
  void BindUniforms(Essentials::PhysicsType objectPhysicsType) const;

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
  void InitDefaultShape(Essentials::PhysicsType objectPhysicsType,
                        uint32_t particleRadius);
  void GenerateForces(Uniform<glm::mat4, float> shape_model,
                      Uniform<uint32_t> shape_color_type,
                      Essentials::PhysicsType objectPhysicsType);
  void Calculate(uint32_t work_groups, bool create_snapshot);
};
