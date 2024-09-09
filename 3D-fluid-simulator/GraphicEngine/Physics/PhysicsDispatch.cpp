#include "PhysicsDispatch.h"

#include <imgui.h>
#include <HydroTest.h>

uint64_t PhysicsDispatch::_timestamp{};
float PhysicsDispatch::_dt{};

PhysicsDispatch::PhysicsDispatch(glm::ivec3 dimensions)
	:_meshDimensions{dimensions},
	_particleMesh{ static_cast<uint64_t>(dimensions.x * dimensions.y * dimensions.z) } {
	_physicsGenerator.AddShader(GL_COMPUTE_SHADER, "/Element.comp");
	_physicsGenerator.AddShader(GL_COMPUTE_SHADER, "/InitDefaultShape.glsl");
	_physicsGenerator.AddShader(GL_COMPUTE_SHADER, "/Gravitation.glsl");
	_physicsGenerator.AddShader(GL_COMPUTE_SHADER, "/Hydrodynamics.glsl");
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

glm::uvec3 const& PhysicsDispatch::GetMeshDimensions() const
{
	return _meshDimensions;
}

void PhysicsDispatch::UpdateMeshDimensions(glm::uvec3 dimensions)
{
	if (_meshDimensions != dimensions) {
		_meshDimensions = dimensions;
		_meshDimensions = dimensions * 10U;
		_particleMesh.SetBufferMemorySize(_meshDimensions.x * _meshDimensions.y * _meshDimensions.z);
	}
}

void PhysicsDispatch::InitDefaultShape(DistributionShape initObjectBounds, PhysicsType objectPhysicsType, uint32_t shapeRadius)
{
	_timestamp = 0U;
	Bind();

	GLint simulatorStateLoc = glGetProgramResourceLocation(_physicsGenerator.ID(), GL_UNIFORM, "SimulatorState");
	GLint distributionShapeLoc = glGetProgramResourceLocation(_physicsGenerator.ID(), GL_UNIFORM, "DistributionShape");
	GLint physicsTypeLoc = glGetProgramResourceLocation(_physicsGenerator.ID(), GL_UNIFORM, "physicsType");
	GLint shapeRadiusLoc = glGetProgramResourceLocation(_physicsGenerator.ID(), GL_UNIFORM, "shapeRadius");
	if (simulatorStateLoc != -1) {
		glUniform1ui(simulatorStateLoc, static_cast<uint32_t>(SimulationState::INIT));
	}
	if (distributionShapeLoc != -1) {
		glUniform1i(distributionShapeLoc, static_cast<int32_t>(initObjectBounds));
	}
	if (physicsTypeLoc != -1) {
		glUniform1ui(physicsTypeLoc, static_cast<uint32_t>(objectPhysicsType));
	}
	if (shapeRadiusLoc != -1) {
		glUniform1ui(shapeRadiusLoc, static_cast<uint32_t>(shapeRadius));
	}
	_(glDispatchCompute(_meshDimensions.x / 10, _meshDimensions.y / 10, _meshDimensions.z / 10));
	_(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT));
	auto lookupBuffer = _particleMesh.GetBufferSubData(0U, _meshDimensions.x * _meshDimensions.y * _meshDimensions.z);

	Vector particleQn[1000];
	PhysicsProperties* lookupTest = Init();
	for (int i = 0; i < 1000; i++)
	{
		particleQn[i] = GenerateHydrodynamics(i);
	}
	
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

void PhysicsDispatch::GenerateForces(PhysicsType objectPhysicsType)
{
	Bind();

	GLint simulatorStateLoc = glGetProgramResourceLocation(_physicsGenerator.ID(), GL_UNIFORM, "SimulatorState");
	GLint physicsTypeLoc = glGetProgramResourceLocation(_physicsGenerator.ID(), GL_UNIFORM, "physicsType");
	GLint shapeRadiusLoc = glGetProgramResourceLocation(_physicsGenerator.ID(), GL_UNIFORM, "shapeRadius");
	GLint dtLoc = glGetProgramResourceLocation(_physicsGenerator.ID(), GL_UNIFORM, "dt");

	if (simulatorStateLoc != -1) {
		glUniform1ui(simulatorStateLoc, static_cast<uint32_t>(SimulationState::SIMULATION));
	}
	if (physicsTypeLoc != -1) {
		glUniform1ui(physicsTypeLoc, static_cast<uint32_t>(objectPhysicsType));
	}
	if (shapeRadiusLoc != -1) {
		glUniform1ui(shapeRadiusLoc, static_cast<uint32_t>(1));
	}
	if (dtLoc != -1) {
		glUniform1f(dtLoc, _dt);
	}
	_(glDispatchCompute(_meshDimensions.x / 10, _meshDimensions.y / 10, _meshDimensions.z / 10));
	_(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT));
	//auto lookupBuffer = _particleMesh.GetBufferSubData(0U, _meshDimensions.x * _meshDimensions.y * _meshDimensions.z);
	//for (auto& particle : lookupBuffer) {
	//	std::cout << particle;
	//}
}

void PhysicsDispatch::UpdateDeltaTime()
{
	_timestamp++;
	_dt = 1.f / ImGui::GetIO().Framerate;
}
