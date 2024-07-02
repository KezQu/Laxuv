#include <Particles.h>

Particles::Particles(Shape&& particleShape, std::size_t count)
	:_particleShape(std::exchange(particleShape, Shape())),
	_count{ count }
{
	_particleShape.ChangeSubdivision(1);
}

void Particles::Draw() const
{
	_particleShape.Bind();
	Bind();
	_(glDrawElementsInstanced(_particleShape.GetDrawPrimitive(), _particleShape.GetVA().Size(), _particleShape.GetVA().IndexBufferType(), nullptr, _count));
}

void Particles::Bind() const
{
	GLint distFunctionTypeLoc = glGetProgramResourceLocation(_particleShape.GetRenderer().ID(), GL_UNIFORM, "distributionFunctionType");
	GLint objectSizeLoc = glGetProgramResourceLocation(_particleShape.GetRenderer().ID(), GL_UNIFORM, "objectSize");
	GLint maxParticlesLoc = glGetProgramResourceLocation(_particleShape.GetRenderer().ID(), GL_UNIFORM, "maxParticles");
	
	if (distFunctionTypeLoc != -1) {
		glUniform1i(distFunctionTypeLoc, DistributionType::QUBE);
	}
	if (objectSizeLoc != -1) {
		glm::vec3 boundingBox = _particleShape.GetBoundingBox();
		float objectSize = glm::max(boundingBox.x, glm::max(boundingBox.y, boundingBox.z));
		glUniform1f(objectSizeLoc, objectSize * 2);
	}
	if (maxParticlesLoc != -1) {
		glUniform1i(maxParticlesLoc, _count);
	}
}
