#pragma once

#include <Program.h>
#include <VertexArray.h>
#include <Camera.h>

template<GLenum P>
class Object {
protected:
	VertexArray _vertexArray;
	Program _renderer;
	glm::vec3 _translation{ 0.f }, _scale{ 1.f }, _rotation{ 0.f }, _normal{ 0.f }, _center{ 0.f };
public:
	GLenum primitive = P;
protected:
	Object(Program&& renderer, VertexArray&& vertexArray);
	Object(VertexArray&& vertexArray);
	Object(Program&& renderer);
public:
	void Draw() const;
	Object& Move(glm::vec3 translation);
	Object& Scale(glm::vec3 scale);
	Object& Rotate(glm::vec3 rotation);
	glm::mat4 Model() const;
};

template<GLenum P>
inline Object<P>::Object(VertexArray&& vertexArray)
	:_vertexArray{ std::move(vertexArray) }
{}
template<GLenum P>
inline Object<P>::Object(Program&& renderer, VertexArray&& vertexArray)
	: _vertexArray{ std::move(vertexArray) },
	_renderer{ std::move(renderer) }
{}
template<GLenum P>
inline Object<P>::Object(Program&& renderer)
	: _renderer{ std::move(renderer) }
{}
template<GLenum P>
inline void Object<P>::Draw() const
{
	_vertexArray.Bind();
	if (!_renderer.isLinked())
		_renderer.Link();
	_renderer.Bind();

	GLint modelLocation = glGetProgramResourceLocation(_renderer.ID(), GL_UNIFORM, "model");
	GLint viewLocation = glGetProgramResourceLocation(_renderer.ID(), GL_UNIFORM, "view");
	GLint projectionLocation = glGetProgramResourceLocation(_renderer.ID(), GL_UNIFORM, "projection");
	GLint centerLocation = glGetProgramResourceLocation(_renderer.ID(), GL_UNIFORM, "center");
	
	if(modelLocation != -1)
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Model()));
	if (viewLocation != -1)
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(Camera::GetCamera().View()));
	if (projectionLocation != -1)
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(Camera::GetCamera().Projection()));
	if (centerLocation != -1)
		glUniform3fv(centerLocation, 1, glm::value_ptr(_center));

	glDrawElements(primitive, _vertexArray.Size(), _vertexArray.IndexBufferType(), nullptr);
}
template<GLenum P>
inline Object<P>& Object<P>::Move(glm::vec3 translation) {
	_translation = translation;
	return *this;
}
template<GLenum P>
inline Object<P>& Object<P>::Scale(glm::vec3 scale) {
	_scale = scale;
	return *this;
}
template<GLenum P>
inline Object<P>& Object<P>::Rotate(glm::vec3 rotation) {
	_rotation = rotation;
	return *this;
}
template<GLenum P>
inline glm::mat4 Object<P>::Model() const {
	auto I = glm::identity<glm::mat4>();
	auto angle = glm::max(glm::max(_rotation.x, _rotation.y), _rotation.z);
	auto T = glm::dot(_translation, _translation) != 0.f ? glm::translate(I, _translation) : I;
	auto R = glm::dot(_rotation, _rotation) != 0.f ? glm::rotate(I, glm::radians(angle), glm::normalize(_rotation)) : I;
	auto S = glm::dot(_scale, _scale) != 0.f ? glm::scale(I, _scale) : I;
	return T * R * S;
}