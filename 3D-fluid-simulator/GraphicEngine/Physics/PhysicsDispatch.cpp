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
	_particleMesh.Bind(_physicsGenerator.ID());
}

ShaderStorageBuffer<PhysicsProperties> const& PhysicsDispatch::GetParticleMeshBuffer() const
{
	return _particleMesh;
}

glm::ivec3 const& PhysicsDispatch::GetMeshDimensions() const
{
	return _boundingDimensions;
}

void PhysicsDispatch::UpdateMeshDimensions(glm::ivec3 dimensions)
{
	if (_boundingDimensions != dimensions) {
		_boundingDimensions = dimensions * 10;
		_particleMesh.SetBufferMemorySize(_boundingDimensions.x * _boundingDimensions.y * _boundingDimensions.z);
	}
}

void PhysicsDispatch::InitDefaultShape(DistributionShape initObjectBounds, PhysicsType objectPhysicsType)
{
	_timestamp = 0U;
	Bind();

	GLint simulatorStateLoc = glGetProgramResourceLocation(_physicsGenerator.ID(), GL_UNIFORM, "SimulatorState");
	GLint distributionShapeLoc = glGetProgramResourceLocation(_physicsGenerator.ID(), GL_UNIFORM, "DistributionShape");
	GLint physicsTypeLoc = glGetProgramResourceLocation(_physicsGenerator.ID(), GL_UNIFORM, "physicsType");
	if (simulatorStateLoc != -1) {
		glUniform1ui(simulatorStateLoc, static_cast<uint32_t>(SimulationState::INIT));
	}
	if (distributionShapeLoc != -1) {
		glUniform1i(distributionShapeLoc, static_cast<int32_t>(initObjectBounds));
	}
	if (physicsTypeLoc != -1) {
		glUniform1ui(physicsTypeLoc, static_cast<uint32_t>(objectPhysicsType));
	}

	_(glDispatchCompute(_boundingDimensions.x / 10, _boundingDimensions.y / 10, _boundingDimensions.z / 10));
	_(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT));
	//auto lookupBuffer = _particleMesh.GetBufferSubData(0U, _boundingDimensions.x * _boundingDimensions.y * _boundingDimensions.z);
	//for (auto& particle : lookupBuffer) {
	//	std::cout << particle;
	//}
	//for (int i = 0; i < lookupBuffer.size(); i++) {
	//	if (lookupBuffer[i].position.x != _particleMesh.Size()) {
	//		std::cout << lookupBuffer[i-1].position.x;
	//		std::cout << lookupBuffer[i].position.x;
	//		__debugbreak();
	//	}
	//}
}

void PhysicsDispatch::GenerateForces()
{
	Bind();

	GLint simulatorStateLoc = glGetProgramResourceLocation(_physicsGenerator.ID(), GL_UNIFORM, "SimulatorState");
	GLint dtLoc = glGetProgramResourceLocation(_physicsGenerator.ID(), GL_UNIFORM, "dt");
	if (simulatorStateLoc != -1) {
		glUniform1ui(simulatorStateLoc, static_cast<uint32_t>(SimulationState::SIMULATION));
	}
	if (dtLoc != -1) {
		glUniform1f(dtLoc, _dt);
	}
	_(glDispatchCompute(_boundingDimensions.x / 10, _boundingDimensions.y / 10, _boundingDimensions.z / 10));
	_(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT));
}

void PhysicsDispatch::UpdateDeltaTime()
{
	_timestamp++;
	_dt = 1.f / ImGui::GetIO().Framerate;
}
