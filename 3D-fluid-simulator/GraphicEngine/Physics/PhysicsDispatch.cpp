#include "PhysicsDispatch.h"


PhysicsDispatch::PhysicsDispatch() {
	_forcesQueue.AddShader(GL_COMPUTE_SHADER, "/Gravitation.comp");
	_initState.AddShader(GL_COMPUTE_SHADER, "/InitDefaultState.comp");
}
void PhysicsDispatch::CopyDataToLocalBuffer(uint32_t bufferId, std::vector<PhysicsProperties>& localBuffer) {
	_(glGetNamedBufferSubData(bufferId, 0, localBuffer.size() * sizeof(PhysicsProperties), localBuffer.data()));
}
void PhysicsDispatch::Bind(Program const& workingShaderProgram) const
{
	_particleMesh.Bind();
	if (!workingShaderProgram.isLinked())
		workingShaderProgram.Link();
	workingShaderProgram.Bind();
	//auto bindingIndex = glGetUniformBlockIndex(workingShaderProgram.ID(), "dataBuffer");
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _particleMesh.ID());
}
std::shared_ptr<PhysicsDispatch>& PhysicsDispatch::GetDispatchInstance()
{
	static std::shared_ptr<PhysicsDispatch> instance(new PhysicsDispatch());
	return instance;
}

void PhysicsDispatch::InitDefaultState(Distribution distribution, glm::vec3 const& location, uint64_t maxParticles)
{
	if (distribution == Distribution::DISK) {
		maxParticles = std::sqrt(maxParticles / glm::pi<float>());
	}
	else if (distribution == Distribution::SPHERE) {
		maxParticles = std::cbrt(3 * maxParticles / (4 * glm::pi<float>()));
	}
	_particleMesh.Add(std::vector<PhysicsProperties>(maxParticles));
	
	_particleMesh.Bind();
	if (!_initState.isLinked())
		_initState.Link();
	_initState.Bind();
	
	auto bindingIndex = glGetUniformBlockIndex(_initState.ID(), "dataBuffer");
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _particleMesh.ID());	

	GLint distFunctionTypeLoc = glGetProgramResourceLocation(_initState.ID(), GL_UNIFORM, "distributionFunctionType");
	if (distFunctionTypeLoc != -1) {
		glUniform1i(distFunctionTypeLoc, static_cast<int32_t>(distribution));
	}
	
	glDispatchCompute(maxParticles, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	CopyDataToLocalBuffer(_particleMesh.ID(), _particleMesh.Data());
}

void PhysicsDispatch::Reset()
{
	_timestamp = 0U;
}

void PhysicsDispatch::GenerateFrameForces()
{
	_timestamp++;

	_particleMesh.Bind();
	if (!_forcesQueue.isLinked())
		_forcesQueue.Link();
	_forcesQueue.Bind();
	auto bindingIndex = glGetUniformBlockIndex(_forcesQueue.ID(), "dataBuffer");
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _particleMesh.ID());
	glDispatchCompute(_particleMesh.Size(), 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	CopyDataToLocalBuffer(_particleMesh.ID(), _particleMesh.Data());
}

void PhysicsDispatch::AddListener(std::function<bool()> const& listenerCallback)
{
}


//std::function<std::vector<PhysicsProperties>()> fluidDistributionFunction;
//switch (distribution) {
//case Distribution::LINE:
//	fluidDistributionFunction = [location, maxParticles, grain]() {
//		std::vector<PhysicsProperties>fluidParticlesPositions(maxParticles);
//		std::for_each(std::execution::par_unseq, fluidParticlesPositions.begin(), fluidParticlesPositions.end(), [&](auto& value) {
//			static int init = -maxParticles / 2;
//			value.position = glm::vec4(location, 0) + glm::vec4(
//				0.f,
//				init, 
//				0.f, 
//				0.f) * grain;
//			init++;
//			});
//		return fluidParticlesPositions; };
//	break;
//case Distribution::CIRCLE:
//	fluidDistributionFunction = [location, maxParticles, grain]() {
//		std::vector<PhysicsProperties>fluidParticlesPositions(maxParticles);
//		std::for_each(std::execution::par_unseq, fluidParticlesPositions.begin(), fluidParticlesPositions.end(), [&](auto& value) {
//			static int init = 0;
//			static float const angle = 2.f * glm::pi<float>() / maxParticles;
//			static float const radius =  maxParticles / (2.f * glm::pi<float>());
//			value.position = glm::vec4(location, 0) + glm::vec4(
//				radius * std::cos(angle * init),
//				radius * std::sin(angle * init),
//				0.f,
//				0.f) * grain;
//			init++;
//			});
//		return fluidParticlesPositions; };
//	break;
//case Distribution::SQUARE:
//	fluidDistributionFunction = [location, maxParticles, grain]() {
//		std::vector<PhysicsProperties>fluidParticlesPositions(maxParticles);
//		std::for_each(std::execution::par_unseq, fluidParticlesPositions.begin(), fluidParticlesPositions.end(), [&](auto& value) {
//			static int init = 0;
//			static float const edgeLength = std::sqrt(maxParticles);
//			value.position = glm::vec4(location, 0) + glm::vec4(
//				std::floor(init / edgeLength),
//				std::fmod(init, edgeLength),
//				0.f,
//				0.f) * grain;
//			init++;
//			});
//		return fluidParticlesPositions; };
//	break;
//case Distribution::DISK:
//	fluidDistributionFunction = [location, maxParticles, grain]() {
//		std::vector<PhysicsProperties>fluidParticlesPositions(maxParticles);
//		std::for_each(std::execution::par_unseq, fluidParticlesPositions.begin(), fluidParticlesPositions.end(), [&](auto& value) {
//			static int init = 0;
//			static float const radius = std::sqrt(maxParticles / glm::pi<float>());
//			auto candidatePosition = glm::vec4(
//				std::floor(init / (radius * 2)), 
//				std::fmod(init, radius * 2), 
//				0.f,
//				0.f);
//			while (glm::length(candidatePosition) > radius) { 
//				init++;
//				candidatePosition = glm::vec4(
//					std::floor(init / (radius * 2)), 
//					std::fmod(init, radius * 2), 
//					0.f,
//					0.f);
//			}
//			value.position = glm::vec4(location, 0) + candidatePosition * grain;
//			init++;
//			});
//		return fluidParticlesPositions; };
//	break;
//case Distribution::QUBE:
//	fluidDistributionFunction = [location, maxParticles, grain]() {
//		std::vector<PhysicsProperties>fluidParticlesPositions(maxParticles);
//		std::for_each(std::execution::par_unseq, fluidParticlesPositions.begin(), fluidParticlesPositions.end(), [&](auto& value) {
//			static int init = 0;
//			static float const edgeLength = std::cbrt(maxParticles);
//			value.position = glm::vec4(location, 0) + glm::vec4(
//				std::floor(init / edgeLength), 
//				std::fmod(init, edgeLength), 
//				std::floor(init / (edgeLength * edgeLength)),
//				0.f) * grain;
//			init++;
//			});
//		return fluidParticlesPositions; };
//	break;
//case Distribution::SPHERE:
//	fluidDistributionFunction = [location, maxParticles, grain]() {
//		std::vector<PhysicsProperties>fluidParticlesPositions(maxParticles);
//		std::for_each(std::execution::par_unseq, fluidParticlesPositions.begin(), fluidParticlesPositions.end(), [&](auto& value) {
//			static int init = 0;
//			static float const radius = std::cbrt((3.f * maxParticles) / (4 * glm::pi<float>()));
//			auto candidatePosition = glm::vec4(
//				std::floor(init / (radius * 2)),
//				std::fmod(init, (radius * 2)),
//				std::floor(init / (radius * radius * 4)),
//				0.f);
//			while (glm::length(candidatePosition) > radius) {
//				init++;
//				candidatePosition = glm::vec4(
//					std::floor(init / (radius * 2)), 
//					std::fmod(init, (radius * 2)), 
//					std::floor(init / (radius * radius * 4)),
//					0.f);
//			}
//			value.position = glm::vec4(location, 0) + candidatePosition * grain;
//			init++;
//			});
//		return fluidParticlesPositions; };
//	break;
//}

