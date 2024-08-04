#pragma once


#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <GL/glew.h>

#include "Program.h"
#include "Essentials.h"
#include "ShaderStorageBuffer.h"

class PhysicsDispatch{
private:
	ShaderStorageBuffer<PhysicsProperties> _particleMesh;
	Program _physicsGenerator;
	glm::ivec3 _boundingDimensions;
	static uint64_t _timestamp;
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

	glm::ivec3& GetMeshDimensions();
	void UpdateMeshDimensions(glm::ivec3 dimensions);
	void InitDefaultShape(DistributionShape initObjectBounds);
	void GenerateForces();
	void BindSSBOToProgram(uint64_t const& programID) const;
};
