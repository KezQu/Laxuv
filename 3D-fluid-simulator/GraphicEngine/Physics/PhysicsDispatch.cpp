#include "PhysicsDispatch.h"
#include <imgui.h>

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

ShaderStorageBuffer<Essentials::PhysicsProperties> const& PhysicsDispatch::GetParticleMeshBuffer() const
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

void PhysicsDispatch::InitDefaultShape(Essentials::DistributionShape initObjectBounds, Essentials::PhysicsType objectPhysicsType, uint32_t shapeRadius)
{
	_timestamp = 0U;
	Bind();

	GLint simulatorStateLoc = glGetProgramResourceLocation(_physicsGenerator.ID(), GL_UNIFORM, "SimulatorState");
	GLint distributionShapeLoc = glGetProgramResourceLocation(_physicsGenerator.ID(), GL_UNIFORM, "DistributionShape");
	GLint physicsTypeLoc = glGetProgramResourceLocation(_physicsGenerator.ID(), GL_UNIFORM, "physicsType");
	GLint shapeRadiusLoc = glGetProgramResourceLocation(_physicsGenerator.ID(), GL_UNIFORM, "shapeRadius");
	GLint kernelRadiusLoc = glGetProgramResourceLocation(_physicsGenerator.ID(), GL_UNIFORM, "KernelRadius");
	if (simulatorStateLoc != -1) {
		glUniform1ui(simulatorStateLoc, static_cast<uint32_t>(Essentials::SimulationState::INIT));
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
	if (kernelRadiusLoc != -1) {
		glUniform1ui(kernelRadiusLoc, static_cast<uint32_t>(4));
	}
	_(glDispatchCompute(_meshDimensions.x / 10, _meshDimensions.y / 10, _meshDimensions.z / 10));
	//_(glDispatchCompute(_meshDimensions.x, _meshDimensions.y, _meshDimensions.z));
	_(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT));
	auto lookupBuffer = _particleMesh.GetBufferSubData(0U, _meshDimensions.x * _meshDimensions.y * _meshDimensions.z);
	
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
	//testing_suite.Init();
}

void PhysicsDispatch::GenerateForces(Essentials::PhysicsType objectPhysicsType)
{
	Bind();

	GLint simulatorStateLoc = glGetProgramResourceLocation(_physicsGenerator.ID(), GL_UNIFORM, "SimulatorState");
	GLint physicsTypeLoc = glGetProgramResourceLocation(_physicsGenerator.ID(), GL_UNIFORM, "physicsType");
	GLint shapeRadiusLoc = glGetProgramResourceLocation(_physicsGenerator.ID(), GL_UNIFORM, "shapeRadius");
	GLint dtLoc = glGetProgramResourceLocation(_physicsGenerator.ID(), GL_UNIFORM, "dt");

	if (simulatorStateLoc != -1) {
		glUniform1ui(simulatorStateLoc, static_cast<uint32_t>(Essentials::SimulationState::SIMULATION));
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
	_(glDispatchCompute(_meshDimensions.x / 10., _meshDimensions.y / 10., _meshDimensions.z / 10.));
	//_(glDispatchCompute(_meshDimensions.x, _meshDimensions.y, _meshDimensions.z));
	_(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT));
	auto lookupBuffer = _particleMesh.GetBufferSubData(0U, _meshDimensions.x * _meshDimensions.y * _meshDimensions.z);
	
	//Vector Q_n1[Essentials::MaxParticles];
	//for (int i = 0; i < Essentials::MaxParticles; i++)
	//{
	//	Q_n1[i] = testing_suite.GenerateHydrodynamics(i);
	//}
	//for (int i = 0; i < Essentials::MaxParticles; i++)
	//{
	//	testing_suite.particle[i].velocity = glm::vec4(Q_n1[i].y / Q_n1[i].x, 0);
	//	testing_suite.particle[i].position = testing_suite.particle[i].position + testing_suite.particle[i].velocity * _dt;
	//}
	//for (int i = 0; i < Essentials::MaxParticles; i++)
	//{
	//	testing_suite.FindNeighbours(i, Essentials::MaxParticles);
	//}
	//for (int i = 0; i < Essentials::MaxParticles; i++)
	//{
	//	const float volume = 1. / testing_suite.CalculateOmega(i);
	//	int n = 0;
	//	while (testing_suite.particle[i].neighbours[n] != 0xffff) n++;
	//	const float pressure = testing_suite.R * (n + 1) * 273.f / volume;
	//	testing_suite.particle[i].VolumeDensityPressureMass.x = volume;
	//	testing_suite.particle[i].VolumeDensityPressureMass.y = Q_n1[i].x / volume;
	//	testing_suite.particle[i].VolumeDensityPressureMass.z = pressure;
	//	testing_suite.particle[i].VolumeDensityPressureMass.w = Q_n1[i].x;
	//}
}

void PhysicsDispatch::UpdateDeltaTime()
{
	_timestamp++;
	//_dt = 1.f / ImGui::GetIO().Framerate;
	_dt = 0.01f;
}
