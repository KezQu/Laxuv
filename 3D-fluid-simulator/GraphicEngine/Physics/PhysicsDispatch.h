#pragma once


#include <glm/vec3.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/geometric.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <GL/glew.h>

#include "Program.h"
#include "Essentials.h"
#include "ShaderStorageBuffer.h"
#include <HydroTest.h>

class PhysicsDispatch{
private:
	ShaderStorageBuffer<Essentials::PhysicsProperties> _particleMesh;
	Program _physicsGenerator;
	glm::uvec3 _meshDimensions;
	static uint64_t _timestamp;	
	HydroTest testing_suite{};
	static float _dt;
private:
	void Bind() const;
public:
	PhysicsDispatch(glm::ivec3 dimensions);
	PhysicsDispatch(PhysicsDispatch const& obj_copy) = delete;
	PhysicsDispatch(PhysicsDispatch&& obj_move) = default;
	PhysicsDispatch& operator=(PhysicsDispatch const& obj_copy) = delete;
	PhysicsDispatch& operator=(PhysicsDispatch&& obj_move) = default;
	~PhysicsDispatch() = default;

	ShaderStorageBuffer<Essentials::PhysicsProperties> const& GetParticleMeshBuffer() const;
	glm::uvec3 const& GetMeshDimensions() const;
	void UpdateMeshDimensions(glm::uvec3 dimensions);
	void InitDefaultShape(Essentials::DistributionShape initObjectBounds, Essentials::PhysicsType objectPhysicsType, uint32_t shapeRadius);
	void GenerateForces(Essentials::PhysicsType objectPhysicsType);
	static void UpdateDeltaTime();
};
