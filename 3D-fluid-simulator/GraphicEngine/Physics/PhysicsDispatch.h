#pragma once


#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <GL/glew.h>

#include "Program.h"
#include "Properties.h"
#include "ShaderStorageBuffer.h"

#include <functional>
#include <execution>
#include <numeric>
#include <cmath>

enum class Distribution {
	LINE,
	CIRCLE,
	SQUARE,
	DISK,
	QUBE,
	SPHERE
};

class PhysicsDispatch{
private:
	Program _forcesQueue;
	Program _initState;
	mutable uint64_t _timestamp;
	ShaderStorageBuffer<PhysicsProperties> _particleMesh;
private:
	PhysicsDispatch();
	void CopyDataToLocalBuffer(uint32_t bufferId, std::vector<PhysicsProperties>& localBuffer);
	void Bind(Program const& workingShaderProgram) const;
public:
	static std::shared_ptr<PhysicsDispatch>& GetDispatchInstance();
	void InitDefaultState(Distribution distribution, glm::vec3 const& location, uint64_t maxParticles);
	void Reset();
	void GenerateFrameForces();
	void AddListener(std::function<bool()> const& listenerCallback);
};
