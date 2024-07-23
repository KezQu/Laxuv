//#include <Particles.h>
//
//Particles::Particles(Shape&& particleShape, std::size_t count)
//	:_particleShape(std::exchange(particleShape, Shape())),
//	_count{ count }
//{
//	_particleShape.ChangeSubdivision() = 1;
//}
//
//void Particles::Draw() const
//{
//	if (_visible) {
//		_particleShape.Bind();
//		Bind();
//		_(glDrawElementsInstanced(_particleShape.GetDrawPrimitive(), _particleShape.GetVA().Size(), _particleShape.GetVA().IndexBufferType(), nullptr, _count));
//	} 
//}
//
//void Particles::Bind() const
//{
//	GLint distFunctionTypeLoc = glGetProgramResourceLocation(_particleShape.GetRenderer().ID(), GL_UNIFORM, "distributionFunctionType");
//	GLint objectSizeLoc = glGetProgramResourceLocation(_particleShape.GetRenderer().ID(), GL_UNIFORM, "objectSize");
//	GLint maxParticlesLoc = glGetProgramResourceLocation(_particleShape.GetRenderer().ID(), GL_UNIFORM, "maxParticles");
//	
//	if (distFunctionTypeLoc != -1) {
//		glUniform1i(distFunctionTypeLoc, _distribution);
//	}
//	if (objectSizeLoc != -1) {
//		glm::vec3 boundingBox = _particleShape.GetBoundingBox();
//		float objectSize = glm::max(boundingBox.x, glm::max(boundingBox.y, boundingBox.z));
//		glUniform1f(objectSizeLoc, objectSize * 2);
//	}
//	if (maxParticlesLoc != -1) {
//		glUniform1i(maxParticlesLoc, _count);
//	}
//}
//Particles::details_map Particles::Details()
//{
//	details_map details = Entity::Details();
//	details.push_back({ "Location", { [=]() {return std::ref(this->_particleShape.Move()); }, DetailsType::VEC3 } });
//	details.push_back({ "Rotation", { [=]() {return std::ref(this->_particleShape.Rotate()); }, DetailsType::VEC3 } });
//	details.push_back({ "Scale", { [=]() {return std::ref(this->_particleShape.Scale()); }, DetailsType::VEC3 } });
//	details.push_back({ "Light", { [=]() {return std::ref(this->_particleShape.EnableLight()); }, DetailsType::BOOL } });
//	details.push_back({ "Subdivision", { [=]() {return std::ref(this->_particleShape.ChangeSubdivision()); }, DetailsType::INT } });
//	details.push_back({ "Distribution", { [=]() {return std::ref(this->Distribution()); }, DetailsType::INT } });
//	details.push_back({ "Count", { [=]() {return std::ref(this->Count()); }, DetailsType::SIZET } });
//	return details;
//}
//
//int& Particles::Distribution()
//{
//	return _distribution;
//}
//
//std::size_t& Particles::Count()
//{
//	return _count;
//}
