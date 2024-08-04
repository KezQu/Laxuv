#include "PhysicsDispatch.h"

#include <imgui.h>

uint64_t PhysicsDispatch::_timestamp{};
float PhysicsDispatch::_dt{};

PhysicsDispatch::PhysicsDispatch(glm::ivec3 dimensions)
	:_boundingDimensions{dimensions},
	_particleMesh{ static_cast<uint64_t>(dimensions.x * dimensions.y * dimensions.z) } {
	_physicsGenerator.AddShader(GL_COMPUTE_SHADER, "/Element.comp");
	_physicsGenerator.AddShader(GL_COMPUTE_SHADER, "/InitDefaultShape.glsl");
	_physicsGenerator.AddShader(GL_COMPUTE_SHADER, "/Gravitation.glsl");
}

void PhysicsDispatch::Bind() const
{
	if (!_physicsGenerator.isLinked())
		_physicsGenerator.Link();
	_physicsGenerator.Bind();
	BindSSBOToProgram(_physicsGenerator.ID());
}

glm::ivec3& PhysicsDispatch::GetMeshDimensions()
{
	return _boundingDimensions;
}

void PhysicsDispatch::UpdateMeshDimensions(glm::ivec3 dimensions)
{
	if (_boundingDimensions != dimensions) {
		_boundingDimensions = dimensions;
		_particleMesh.SetBufferMemorySize(dimensions.x * dimensions.y * dimensions.z);
	}
}

void PhysicsDispatch::InitDefaultShape(DistributionShape initOjectBounds)
{
	_timestamp = 0U;
 	Bind();

	GLint simulatorStateLoc = glGetProgramResourceLocation(_physicsGenerator.ID(), GL_UNIFORM, "SimulatorState");
	if (simulatorStateLoc != -1) {
		glUniform1ui(simulatorStateLoc, static_cast<uint32_t>(SimulationState::INIT));
	}
	
	GLint distributionShapeLoc = glGetProgramResourceLocation(_physicsGenerator.ID(), GL_UNIFORM, "DistributionShape");
	if (distributionShapeLoc != -1) {
		glUniform1i(distributionShapeLoc, static_cast<int32_t>(initOjectBounds));
	}
	
	//glDispatchCompute(_boundingDimensions.x, _boundingDimensions.y, _boundingDimensions.z);
	_(glDispatchCompute(_boundingDimensions.x / 10, _boundingDimensions.y / 10, _boundingDimensions.z / 10));
	_(glMemoryBarrier(GL_ALL_BARRIER_BITS));
	auto lookupBuffer = _particleMesh.GetBufferSubData(1000U, 1050U);
	for (auto& particle : lookupBuffer) {
		std::cout << particle;
	}
}

void PhysicsDispatch::GenerateForces()
{
	_timestamp++;
	_dt = 1.f / ImGui::GetIO().Framerate;

	Bind();

	GLint simulatorStateLoc = glGetProgramResourceLocation(_physicsGenerator.ID(), GL_UNIFORM, "SimulatorState");
	if (simulatorStateLoc != -1) {
		glUniform1ui(simulatorStateLoc, static_cast<uint32_t>(SimulationState::SIMULATION));
	}
	_(glDispatchCompute(_boundingDimensions.x / 10, _boundingDimensions.y / 10, _boundingDimensions.z / 10));
	_(glMemoryBarrier(GL_ALL_BARRIER_BITS));
}

void PhysicsDispatch::BindSSBOToProgram(uint64_t const& programID) const
{
	_particleMesh.Bind();
	auto bindingIndex = glGetProgramResourceIndex(programID, GL_SHADER_STORAGE_BLOCK, "dataBuffer");
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, _particleMesh.ID());
}
